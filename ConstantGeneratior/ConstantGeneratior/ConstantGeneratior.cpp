#pragma once
#include <algorithm>
#include <windows.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include <tuple>
#include <unordered_set>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// Shader 종류를 나타내는 열거형
enum class EShaderType
{
    Vertex,
    Pixel
};

// HLSL 타입 정보를 C++ 타입 문자열로 매핑하는 함수
// 이 예시는 스칼라, 벡터, 행렬을 간단하게 매핑하며, 필요에 따라 확장할 수 있습니다.
static std::string MapHLSLTypeToCPP(const D3D11_SHADER_TYPE_DESC& typeDesc)
{
    std::string cType = "UNKNOWN";
    if (typeDesc.Class == D3D_SVC_SCALAR)
    {
        switch (typeDesc.Type)
        {
        case D3D_SVT_FLOAT:
            cType = "float";
            break;
        case D3D_SVT_INT:
            cType = "int";
            break;
		case D3D_SVT_UINT:
			cType = "uint32";
			break;
        case D3D_SVT_BOOL:
            cType = "bool";
            break;
        default:
            cType = "UNKNOWN";
            break;
        }
    }
    else if (typeDesc.Class == D3D_SVC_VECTOR)
    {
        // HLSL 벡터 타입 매핑 (예: float3 -> FVector)
        if (typeDesc.Type == D3D_SVT_FLOAT)
        {
            switch (typeDesc.Columns)
            {
            case 4:
                cType = "FVector4";
                break;
            case 3: 
                cType = "FVector";
                break;
            case 2:
                cType = "FVector2D";
                break;
            default:
                cType = "float" + std::to_string(typeDesc.Columns);
                break;
            }
        }
        else if (typeDesc.Type == D3D_SVT_INT)
        {
            cType = "int" + std::to_string(typeDesc.Columns);
        }
        else
        {
            cType = "UNKNOWN";
        }
    }
    else if (typeDesc.Class == D3D_SVC_MATRIX_ROWS || typeDesc.Class == D3D_SVC_MATRIX_COLUMNS)
    {
        // 4x4 행렬을 예로 FMatrix로 매핑
        if (typeDesc.Rows == 4 && typeDesc.Columns == 4)
            cType = "FMatrix";
        else
            cType = "matrix";
    }
    
    // 배열인 경우, Elements가 0보다 크면 배열 표기법 추가 (예: float3[4])
    if (typeDesc.Elements > 0)
    {
        cType += "[" + std::to_string(typeDesc.Elements) + "]";
    }
    
    return cType;
}

// 각 변수의 정보를 저장하는 임시 구조체
// tuple: (StartOffset, Size, VariableName, C++ 타입)
using VarInfo = std::tuple<UINT, UINT, std::string, std::string>;

//-----------------------------------------------------------------------------
// GenerateConstantBufferStructsForShaders
//
// 여러 shaderBlob들을 입력받아 각 쉐이더의 리플렉션 정보를 기반으로 상수 버퍼 구조체 코드를 생성합니다.
// 동일한 상수 버퍼 이름은 중복 생성하지 않고, 모든 정보를 한 번에 outputFilename에 기록합니다.
//-----------------------------------------------------------------------------
static void GenerateConstantBufferStructsForShaders(const std::vector<ID3DBlob*>& shaderBlobs, const std::wstring& outputFilename)
{
    // 중복 정의를 피하기 위해 생성한 상수 버퍼 이름을 저장할 집합
    std::unordered_set<std::string> definedCBs;
    std::ostringstream oss;

    oss << "// Auto-generated constant buffer structures with padding\n";
    oss << "#pragma once\n\n";
    oss << "#include \"HAL/PlatformType.h\"\n";
    oss << "#include \"Math/Matrix.h\"\n";
    oss << "#include \"Math/Vector.h\"\n";
    oss << "#include \"Math/Vector4.h\"\n\n";
    oss << "// NOTE: Generated code - do not modify manually.\n\n";

    // 각 쉐이더 블롭에 대해 처리
    for (const auto& shaderBlob : shaderBlobs)
    {
        // ID3D11ShaderReflection 인터페이스 생성 (DirectX 11용)
        ID3D11ShaderReflection* pReflector = nullptr;
        HRESULT hr = D3DReflect(
            shaderBlob->GetBufferPointer(),
            shaderBlob->GetBufferSize(),
            IID_ID3D11ShaderReflection,
            reinterpret_cast<void**>(&pReflector)
        );
        if (FAILED(hr) || !pReflector)
        {
            std::cerr << "D3DReflect failed for a shader blob." << std::endl;
            continue;
        }

        // 쉐이더 설명 가져오기
        D3D11_SHADER_DESC shaderDesc = {};
        hr = pReflector->GetDesc(&shaderDesc);
        if (FAILED(hr))
        {
            std::cerr << "GetDesc failed." << std::endl;
            pReflector->Release();
            continue;
        }

        // 각 상수 버퍼 처리
        for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
        {
            ID3D11ShaderReflectionConstantBuffer* pCB = pReflector->GetConstantBufferByIndex(i);
            if (!pCB)
                continue;

            D3D11_SHADER_BUFFER_DESC cbDesc = {};
            hr = pCB->GetDesc(&cbDesc);
            if (FAILED(hr))
                continue;

            if (cbDesc.Type != D3D_CT_CBUFFER) continue;

            // 만약 이미 이 상수 버퍼 이름에 대한 구조체를 생성했다면 건너뛰기
            std::string cbName(cbDesc.Name);
            if (definedCBs.find(cbName) != definedCBs.end())
                continue;
            definedCBs.insert(cbName);

            // 변수 정보를 수집합니다.
            std::vector<VarInfo> varInfos;
            for (UINT v = 0; v < cbDesc.Variables; ++v)
            {
                ID3D11ShaderReflectionVariable* pVar = pCB->GetVariableByIndex(v);
                if (!pVar)
                    continue;
                D3D11_SHADER_VARIABLE_DESC varDesc = {};
                hr = pVar->GetDesc(&varDesc);
                if (FAILED(hr))
                    continue;
                ID3D11ShaderReflectionType* pType = pVar->GetType();
                D3D11_SHADER_TYPE_DESC typeDesc = {};
                hr = pType->GetDesc(&typeDesc);
                if (FAILED(hr))
                    continue;

                std::string cppType = MapHLSLTypeToCPP(typeDesc);
                varInfos.push_back(std::make_tuple(varDesc.StartOffset, varDesc.Size, std::string(varDesc.Name), cppType));
            }

            // 변수들을 StartOffset 기준으로 정렬
            std::sort(varInfos.begin(), varInfos.end(), [](const VarInfo& a, const VarInfo& b) {
                return std::get<0>(a) < std::get<0>(b);
            });

            // 구조체 선언: 구조체 이름은 상수 버퍼 이름
            oss << "struct alignas(16) " << cbDesc.Name << "\n{\n";

            UINT currentOffset = 0;
            int padCounter = 0;
            for (const auto& var : varInfos)
            {
                UINT startOffset = std::get<0>(var);
                UINT varSize = std::get<1>(var);
                std::string varName = std::get<2>(var);
                std::string varType = std::get<3>(var);

                // 만약 이전 변수의 끝(currentOffset)과 현재 변수 시작(startOffset) 사이에 갭이 있으면 패딩 추가
                if (currentOffset < startOffset)
                {
                    UINT padSize = startOffset - currentOffset;
                    oss << "    uint8 pad" << padCounter++ << "[" << padSize << "]; // Padding from offset "
                        << currentOffset << " to " << startOffset << "\n";
                    currentOffset = startOffset;
                }

                // 변수 선언
                oss << "    " << varType << " " << varName << "; // offset: " << startOffset << ", size: " << varSize << "\n";
                currentOffset += varSize;
            }

            // 마지막 변수 이후 남은 공간에 대한 패딩 추가
            if (currentOffset < cbDesc.Size)
            {
                UINT padSize = cbDesc.Size - currentOffset;
                oss << "    uint8 pad" << padCounter++ << "[" << padSize << "]; // Padding to end of buffer\n";
            }

            oss << "};\n\n";
        }

        pReflector->Release();
    }

    // enum 생성: definedCBs에 있는 상수 버퍼 이름들을 기반으로 enum 생성
    std::vector<std::string> enumNames(definedCBs.begin(), definedCBs.end());
    // 정렬 기준은 필요에 따라 변경할 수 있음 (여기서는 알파벳 순)
    std::sort(enumNames.begin(), enumNames.end());
    oss << "enum class EShaderConstantBuffer\n{\n";
    for (size_t i = 0; i < enumNames.size(); ++i)
    {
        oss << "    " << enumNames[i] << " = " << i << ",\n";
    }
    oss << "    EShaderConstantBuffer_MAX\n};\n\n";
 

    // enum 변환 함수 생성: enum 값을 문자열로 변환하는 inline 함수
    oss << "inline const TCHAR* EShaderConstantBufferToString(EShaderConstantBuffer e)\n{\n";
    oss << "    switch(e)\n    {\n";
    for (size_t i = 0; i < enumNames.size(); ++i)
    {
        oss << "    case EShaderConstantBuffer::" << enumNames[i] << ": return TEXT(\"" << enumNames[i] << "\");\n";
    }
    oss << "    default: return TEXT(\"unknown\");\n";
    oss << "    }\n}\n\n";

    // 반대 함수: 문자열을 enum 값으로 변환하는 inline 함수
    oss << "inline EShaderConstantBuffer EShaderConstantBufferFromString(const TCHAR* str)\n{\n";
    oss << "#if USE_WIDECHAR\n";
    for (size_t i = 0; i < enumNames.size(); ++i)
    {
        oss << "    if(std::wcscmp(str, TEXT(\"" << enumNames[i] << "\")) == 0) return EShaderConstantBuffer::" << enumNames[i] << ";\n";
    }
    oss << "#else\n";
    for (size_t i = 0; i < enumNames.size(); ++i)
    {
        oss << "    if(std::strcmp(str, \"" << enumNames[i] << "\") == 0) return EShaderConstantBuffer::" << enumNames[i] << ";\n";
    }
    oss << "#endif\n";
    oss << "    return EShaderConstantBuffer::EShaderConstantBuffer_MAX;\n";
    oss << "}\n\n";
    

    // 최종적으로 outputFilename에 한 번에 기록
    std::ofstream ofs(outputFilename, std::ios::out);
    if (!ofs.is_open())
    {
        std::wcerr << "Failed to open output file: " << outputFilename << std::endl;
        return;
    }
    ofs << oss.str();
    ofs.close();
    std::wcout << "Generated constant buffer structures with padding in " << outputFilename << std::endl;
}

//-----------------------------------------------------------------------------
// Helper: 폴더 내의 모든 .hlsl 파일 경로를 반환하는 함수
//-----------------------------------------------------------------------------
static std::vector<std::wstring> EnumerateShaderFiles(const std::wstring& folder)
{
    std::vector<std::wstring> shaderFiles;
    std::wstring searchPattern = folder + L"\\*.hlsl";
    
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                shaderFiles.push_back(folder + L"\\" + findData.cFileName);
            }
        } while (FindNextFileW(hFind, &findData));
        FindClose(hFind);
    }
    return shaderFiles;
}

//-----------------------------------------------------------------------------
// Helper: 파일명에 따른 ShaderType 결정 (기본값 Vertex)
//-----------------------------------------------------------------------------
static EShaderType DetermineShaderType(const std::wstring& filePath, EShaderType defaultType = EShaderType::Vertex)
{
    if (filePath.find(L"VS") != std::wstring::npos || filePath.find(L"Vertex") != std::wstring::npos)
        return EShaderType::Vertex;
    if (filePath.find(L"PS") != std::wstring::npos || filePath.find(L"Pixel") != std::wstring::npos)
        return EShaderType::Pixel;
    return defaultType;
}

//-----------------------------------------------------------------------------
// Helper: 컴파일 오류 메시지 출력
//-----------------------------------------------------------------------------
static void PrintShaderError(ID3DBlob* errorBlob)
{
    if (errorBlob)
    {
        std::cerr << "Shader Error: " 
                  << reinterpret_cast<const char*>(errorBlob->GetBufferPointer())
                  << std::endl;
        errorBlob->Release();
    }
}

//-----------------------------------------------------------------------------
// 하나의 쉐이더 파일을 컴파일하는 함수
//-----------------------------------------------------------------------------
static HRESULT CompileShaderFile(const std::wstring& filePath, const EShaderType shaderType, ID3DBlob** ppCode, ID3DBlob** ppErrorBlob)
{
    const char* entryPoint = nullptr;
    const char* target = nullptr;
    switch (shaderType)
    {
    case EShaderType::Vertex:
        entryPoint = "mainVS";
        target = "vs_5_0";
        break;
    case EShaderType::Pixel:
        entryPoint = "mainPS";
        target = "ps_5_0";
        break;
    default:
        return E_INVALIDARG;
    }

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif
    // 최적화 건너뛰기는 디버그 시 유용
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

    return D3DCompileFromFile(
        filePath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        target,
        shaderFlags,
        0,
        ppCode,
        ppErrorBlob
    );
}

// EXE 파일이 위치한 디렉터리를 반환하는 함수
static std::wstring GetExeDirectory()
{
    wchar_t path[MAX_PATH] = {0};
    DWORD length = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (length == 0)
    {
        std::wcerr << L"Failed to get module file name." << std::endl;
        return L"";
    }
    std::wstring exePath(path);
    // 마지막 '\' 또는 '/'까지 자름 (파일 이름 제거)
    size_t pos = exePath.find_last_of(L"\\/");
    if (pos != std::wstring::npos)
    {
        exePath = exePath.substr(0, pos);
    }
    return exePath;
}

//-----------------------------------------------------------------------------
// 폴더 내의 모든 쉐이더 파일을 컴파일하고 리플렉션 정보를 출력하는 함수
//-----------------------------------------------------------------------------
static void CompileAndReflectShaders(const std::vector<std::wstring>& shaderFiles)
{
    std::vector<ID3DBlob*> shaderBlobs;

    for (const auto& filePath : shaderFiles)
    {
        ID3DBlob* shaderBlob = nullptr;
        // 파일명에 따라 ShaderType 결정 (기본은 Vertex)
        const EShaderType shaderType = DetermineShaderType(filePath);

        // 컴파일 결과용 Blob 포인터들
        ID3DBlob* errorBlob = nullptr;

        const HRESULT hr = CompileShaderFile(filePath, shaderType, &shaderBlob, &errorBlob);
        if (FAILED(hr))
        {
            std::wcerr << L"Failed to compile ";
            if (shaderType == EShaderType::Vertex)
                std::wcerr << L"vertex shader: ";
            else
                std::wcerr << L"pixel shader: ";
            std::wcerr << filePath << std::endl;
            PrintShaderError(errorBlob);
        }
        else
        {
            std::wcout << L"Successfully compiled ";
            if (shaderType == EShaderType::Vertex)
                std::wcout << L"vertex shader: ";
            else
                std::wcout << L"pixel shader: ";
            std::wcout << filePath << std::endl;
        }
        shaderBlobs.push_back(shaderBlob);
    }

    std::wstring exeDir = GetExeDirectory();
    std::wcout << L"Exe Directory: " << exeDir << std::endl;
	// 상대 경로 결합 : "../Engine/Engine/Source/Runtime/Windows/D3D11RHI/GPUBuffer/TestConstantDefine.h"
    const std::wstring structFolder = exeDir + L"\\..\\..\\..\\Week0v2\\Engine\\Source\\Runtime\\Windows\\D3D11RHI\\CBStructDefine.h";

    GenerateConstantBufferStructsForShaders(shaderBlobs, structFolder);
    for (const auto item : shaderBlobs)
        item->Release();
    
}


//-----------------------------------------------------------------------------
// main 함수: 특정 폴더의 모든 .hlsl 파일을 컴파일하고 리플렉션 출력
//-----------------------------------------------------------------------------
int main()
{
    std::wstring exeDir = GetExeDirectory();
    std::wcout << L"Exe Directory: " << exeDir << std::endl;

    // EXE 파일의 디렉터리를 기준으로 상대 경로 결합: "../Engine/Shaders"
    std::wstring shaderFolder = exeDir + L"\\..\\..\\..\\Week0v2\\Shaders";
    std::wcout << L"Shader Folder: " << shaderFolder << std::endl;

    // 폴더 내의 .hlsl 파일들을 열거
    std::vector<std::wstring> shaderFiles = EnumerateShaderFiles(shaderFolder);

    // 쉐이더 파일들을 컴파일하고 리플렉션 정보를 출력
    CompileAndReflectShaders(shaderFiles);

    return 0;
}
