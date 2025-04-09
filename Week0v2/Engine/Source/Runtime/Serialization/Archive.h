#pragma once
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <type_traits>


class FArchive
{
	friend class FWindowsBinHelper;
private:
	std::stringstream Stream;

public:
	explicit FArchive()
		: Stream(std::ios::in | std::ios::out | std::ios::binary) 
	{
	}

	bool IsEmpty()
	{
		return Stream.str().empty();
	}

private:
	std::string SaveToBinary() 
	{
		auto PrevPos = Stream.tellg();
		Stream.seekg(0, std::ios::end);
		size_t Size = Stream.tellg();
		Stream.seekg(0, std::ios::beg);

		std::string BinaryData;
		BinaryData.resize(Size);
		Stream.read(&BinaryData[0], Size);

		Stream.seekg(PrevPos);
		return BinaryData;
	}

	void LoadFromBinary(const std::string& InBinaryData)
	{
		Stream.clear();
		Stream.str(InBinaryData);
		Stream.seekg(0);  // 읽기 위치 초기화
		Stream.seekp(0);  // 쓰기 위치 초기화
	}

	void SaveToFile(const std::string& FilePath)
	{
		std::ofstream File(FilePath, std::ios::binary);
		if (File.is_open())
		{
			File << SaveToBinary();
			File.close();
		}
	}

	void LoadFromFile(const std::string& FilePath)
	{
		std::ifstream File(FilePath, std::ios::binary);
		if (File.is_open())
		{
			std::stringstream Buffer;
			Buffer << File.rdbuf();
			LoadFromBinary(Buffer.str());
			File.close();
		}
	}
#pragma region String Convert
    std::string WStringToUTF8(const std::wstring& wstr) {
        if (wstr.empty()) return {};

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0],
            static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0],
            static_cast<int>(wstr.size()), &strTo[0], size_needed, nullptr, nullptr);
        return strTo;
    }

    // UTF-8 string -> wstring
    std::wstring UTF8ToWString(const std::string& str) {
        if (str.empty()) return {};

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0],
            static_cast<int>(str.size()), nullptr, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0],
            static_cast<int>(str.size()), &wstrTo[0], size_needed);
        return wstrTo;
    }
#pragma endregion

#pragma region Operator Overload
public:
	template <typename T>
	typename std::enable_if_t<std::is_fundamental_v<T>, FArchive&> operator<<(const T& Value)
	{
		Stream.write(reinterpret_cast<const char*>(&Value), sizeof(T));
		return *this;
	}

	template<typename T>
	typename std::enable_if_t<std::is_fundamental_v<T>, FArchive&>operator>>(T& Value)
	{
		Stream.read(reinterpret_cast<char*>(&Value), sizeof(T));
		return *this;
	}

	// std::string
	FArchive& operator<<(const std::string& Value)
	{
		size_t Size = Value.size();
		// 1. 크기를 바이너리로 저장
		Stream.write(reinterpret_cast<const char*>(&Size), sizeof(size_t));
		// 2. 문자열 데이터를 바이너리로 저장
		Stream.write(Value.data(), Size);
		return *this;
	}

	FArchive& operator>>(std::string& Value)
	{
		size_t Size;
		// 1. 크기를 바이너리로 읽기
		Stream.read(reinterpret_cast<char*>(&Size), sizeof(size_t));
		// 3. 문자열 데이터 읽기
		Value.resize(Size);
		Stream.read(&Value[0], Size);  // C++11 이상: &Value[0] 대신 Value.data()도 가능
		return *this;
	}

    // std::wstring
    FArchive& operator<<(const std::wstring& Value)
    {
        std::string str = WStringToUTF8(Value);
        *this << str;
        return *this;
    }

    FArchive& operator>>(std::wstring& Value)
    {
        std::string str;
        *this >> str;
        
        Value = UTF8ToWString(str);
        return *this;
    }
	// std::vector<T>
	template<typename T, typename Allocator>
	FArchive& operator<<(const std::vector<T, Allocator>& Value)
	{
		size_t Size = Value.size();
		Stream.write(reinterpret_cast<const char*>(&Size), sizeof(size_t));
		for (const T& Element : Value)
		{
			// !NOTE : T도 Serializable해야 함
			*this << Element;
		}
		return *this;
	}

	template<typename T, typename Allocator>
	FArchive& operator>>(std::vector<T, Allocator>& Value)
	{
		size_t Size;
		Stream.read(reinterpret_cast<char*>(&Size), sizeof(size_t));
		Value.resize(Size);
		for (T& Element : Value)
		{
			// !NOTE : T도 Serializable해야 함
			*this >> Element;
		}
		return *this;
	}

	// std::unordered_map<Key, Value>
	template<typename Key, typename Value>
	FArchive& operator<<(const std::unordered_map<Key, Value>& InValue)
	{
		size_t Size = InValue.size();
		Stream.write(reinterpret_cast<const char*>(&Size), sizeof(size_t));
		for (const auto& Element : InValue)
		{
			*this << Element.first;
			*this << Element.second;
		}
		return *this;
	}

	template<typename Key, typename Value>
	FArchive& operator>>(std::unordered_map<Key, Value>& InValue)
	{
		size_t Size;
		Stream.read(reinterpret_cast<char*>(&Size), sizeof(size_t));
		for (size_t i = 0; i < Size; ++i)
		{
			Key KeyElement;
			Value ValueElement;
			*this >> KeyElement;    
			*this >> ValueElement; 
			InValue.emplace(std::move(KeyElement), std::move(ValueElement)); 
		}
		return *this;
	}

	// std::unordered_set<T>
	template<typename T>
	FArchive& operator<<(const std::unordered_set<T>& InValue)
	{
		size_t Size = InValue.size();
		Stream.write(reinterpret_cast<const char*>(&Size), sizeof(size_t));
		for (const T& Element : InValue)
		{
			*this << Element;
		}
		return *this;
	}

	template<typename T>
	FArchive& operator>>(std::unordered_set<T>& InValue)
	{
		size_t Size;
		Stream.read(reinterpret_cast<char*>(&Size), sizeof(size_t));
		for (size_t i = 0; i < Size; ++i)
		{
			T Element;
			*this >> Element;
			InValue.insert(Element);
		}
		return *this;
	}

	// user-defined type
	template<typename T, typename = decltype(std::declval<T>().Serialize(std::declval<FArchive&>()))>
	FArchive& operator<<(T& InValue)
	{
		InValue.Serialize(*this);
		return *this;
	}

	template<typename T, typename = decltype(std::declval<T>().Deserialize(std::declval<FArchive&>()))>
	FArchive& operator>>(T& InValue)
	{
		InValue.Deserialize(*this);
		return *this;
	}
#pragma endregion
};

