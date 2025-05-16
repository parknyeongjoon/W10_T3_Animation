#pragma once
#include <variant>
#include <optional>

#include "Object.h"
#include "PropertyTypes.h"
#include "Container/Queue.h"
#include "Templates/TemplateUtilities.h"
#include "Templates/TypeUtilities.h"
#include "ThirdParty/include/ImGUI/imgui.h"

struct FProperty
{
    /**
     * 클래스의 프로퍼티를 등록합니다.
     * @param InOwnerClass 이 프로퍼티를 가지고 있는 UClass
     * @param InPropertyName 프로퍼티의 이름
     * @param InType 프로퍼티의 타입
     * @param InSize 프로퍼티의 크기
     * @param InOffset 프로퍼티가 클래스로부터 떨어진 거리
     * @param InFlags Reflection 관련 Flag
     */
    FProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        EPropertyType InType,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : OwnerClass(InOwnerClass)
        , Name(InPropertyName)
        , Type(InType)
        , Size(InSize)
        , Offset(InOffset)
        , Flags(InFlags)
    {
    }

    virtual ~FProperty() = default;
    FProperty(const FProperty&) = default;
    FProperty& operator=(const FProperty&) = default;
    FProperty(FProperty&&) = default;
    FProperty& operator=(FProperty&&) = default;

public:
    /** ImGui에 각 프로퍼티에 맞는 UI를 띄웁니다. */
    virtual void DisplayInImGui(UObject* Object) const;

    /**
     * ImGui를 통해 주어진 Raw Data를 표시합니다.
     * @param PropertyLabel 표시할 프로퍼티의 레이블
     * @param DataPtr 표시할 Raw Data의 포인터
     */
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const;

    /** 런타임에 타입 정보를 검사하고 업데이트 합니다. */
    virtual void Resolve();

    /**
     * 이 프로퍼티가 EPropertyType::Object 타입일 경우, 해당 UObject의 UClass*를 반환합니다.
     * 그렇지 않거나 아직 해결되지 않은 경우 nullptr을 반환합니다.
     * @return UClass* 또는 nullptr
     */
    FORCEINLINE UClass* GetSpecificClass() const
    {
        return GetTypeSpecificDataAs<UClass*>().value_or(nullptr);
    }

private:
    /**
     * TypeSpecificData에서 특정 타입 T의 값을 안전하게 가져옵니다.
     * 만약 현재 저장된 타입이 T가 아니거나, Type이 해당 T를 가질 수 있는
     * EPropertyType 상태가 아니라면 std::nullopt를 반환합니다.
     *
     * @tparam T 가져오고자 하는 타입 (예: UClass*, FStructInfo*, FName)
     * @return 해당 타입의 값을 담은 std::optional<T>, 또는 std::nullopt
     */
    template <typename T>
    [[nodiscard]] std::optional<T> GetTypeSpecificDataAs() const
    {
        if constexpr (std::same_as<T, UClass*>)
        {
            switch (Type)  // NOLINT(clang-diagnostic-switch-enum)
            {
                // Type이 Object와 SubclassOf 일 때만 UClass*가 유효
                case EPropertyType::Object:
                case EPropertyType::SubclassOf:
                    break;
                default:
                    return std::nullopt;
            }
        }
        // else if constexpr (std::same_as<T, FStructInfo*>) {
        //     if (Type != EPropertyType::Struct) { // Struct 타입일 때만 FStructInfo*가 유효
        //         return std::nullopt;
        //     }
        // }
        else if constexpr (std::same_as<T, FName>)
        {
            if (Type != EPropertyType::UnresolvedPointer /* && Type != EPropertyType::UnresolvedStruct 등 */)
            {
                return std::nullopt;
            }
        }

        // 실제 variant에서 값 가져오기 시도
        if (std::holds_alternative<T>(TypeSpecificData))
        {
            return std::get<T>(TypeSpecificData);
        }
        return std::nullopt;
    }

protected:
    /**
     * Property를 실제 데이터 타입으로 변환합니다.
     * @tparam T 변환할 타입
     * @param Object 변환할 값을 가지고 있는 Object
     * @return Object의 실제 값
     *
     * @warning 타입이 잘못되면 UB가 발생할 수 있습니다.
     */
    template <typename T>
    T* GetPropertyData(UObject* Object) const
    {
        return reinterpret_cast<T*>(reinterpret_cast<std::byte*>(Object) + Offset);
    }

    void* GetPropertyData(UObject* Object) const
    {
        return reinterpret_cast<std::byte*>(Object) + Offset;
    }

public:
    UClass* OwnerClass;

    const char* Name;
    EPropertyType Type;
    int64 Size;
    int64 Offset;
    EPropertyFlags Flags;

public:
    std::variant<
        std::monostate,  // 현재 값이 없음을 나타냄
        UClass*,         // FProperty의 Type이 Object일때 원본 Property를 가지고 있는 UClass
        // FStructInfo*,    // FProperty의 Type이 Struct일때 커스텀 구조체의 정보
        FName            // FProperty의 Type이 UnresolvedPointer일 때 런타임에 검사할 UClass 이름
    > TypeSpecificData;
};


struct FNumericProperty : public FProperty
{
    FNumericProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        EPropertyType InType,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, InType, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FInt8Property : public FNumericProperty
{
    FInt8Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int8, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FInt16Property : public FNumericProperty
{
    FInt16Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int16, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FInt32Property : public FNumericProperty
{
    FInt32Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int32, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FInt64Property : public FNumericProperty
{
    FInt64Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int64, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FUInt8Property : public FNumericProperty
{
    FUInt8Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt8, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FUInt16Property : public FNumericProperty
{
    FUInt16Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt16, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FUInt32Property : public FNumericProperty
{
    FUInt32Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt32, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FUInt64Property : public FNumericProperty
{
    FUInt64Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt64, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FFloatProperty : public FNumericProperty
{
    FFloatProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Float, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FDoubleProperty : public FNumericProperty
{
    FDoubleProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Double, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FBoolProperty : public FProperty
{
    FBoolProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Bool, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FStrProperty : public FProperty
{
    FStrProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::String, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FNameProperty : public FProperty
{
    FNameProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Name, InSize, InOffset, InFlags)
    {}

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FVector2DProperty : public FProperty
{
    FVector2DProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Vector2D, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FVectorProperty : public FProperty
{
    FVectorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Vector, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FVector4Property : public FProperty
{
    FVector4Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Vector4, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FRotatorProperty : public FProperty
{
    FRotatorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Rotator, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FQuatProperty : public FProperty
{
    FQuatProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Quat, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FTransformProperty : public FProperty
{
    FTransformProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Transform, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FMatrixProperty : public FProperty
{
    FMatrixProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Matrix, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FColorProperty : public FProperty
{
    FColorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Color, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FLinearColorProperty : public FProperty
{
    FLinearColorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::LinearColor, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

template <typename InArrayType>
struct TArrayProperty : public FProperty
{
    using ElementType = typename InArrayType::ElementType;

    FProperty* ElementProperty = nullptr;

    TArrayProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Array, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override
    {
        FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

        enum class EArrayElementOption : uint8
        {
            Insert,
            Remove,
            Duplicate,
        };

        if (ImGui::TreeNode(PropertyLabel))
        {
            ImGui::BeginDisabled(HasFlag(Flags, EPropertyFlags::VisibleAnywhere));
            {
                TArray<ElementType>* Data = static_cast<TArray<ElementType>*>(DataPtr);

                const ImGuiIO& IO = ImGui::GetIO();
                ImFont* IconFont = IO.Fonts->Fonts[1]; // FEATHER_FONT = 1

                ImGui::Text("Num of Elements: %d", Data->Num());

                ImGui::SameLine();
                ImGui::PushFont(IconFont);
                if (ImGui::Button("\ue9c8"))
                {
                    Data->AddDefaulted();
                }
                ImGui::PopFont();
                ImGui::SetItemTooltip("Add Element");

                ImGui::SameLine();
                ImGui::PushFont(IconFont);
                if (ImGui::Button("\ue9f6"))
                {
                    Data->Empty();
                }
                ImGui::PopFont();
                ImGui::SetItemTooltip("Remove All Elements");

                TQueue<TPair<EArrayElementOption, int32>> OptionQueue;
                for (int32 Index = 0; Index < Data->Num(); ++Index)
                {
                    ElementProperty->DisplayRawDataInImGui(std::format("Idx [{}]", Index).c_str(), &((*Data)[Index]));

                    std::string PopupLabel = std::format("ArrayElementOption##{}", Index);
                    ImGui::SameLine();
                    if (ImGui::Button(std::format("...##{}", Index).c_str()))
                    {
                        ImGui::OpenPopup(PopupLabel.c_str());
                    }

                    if (ImGui::BeginPopup(PopupLabel.c_str()))
                    {
                        if (ImGui::Selectable("Insert"))
                        {
                            OptionQueue.Enqueue(MakePair(EArrayElementOption::Insert, Index));
                        }
                        else if (ImGui::Selectable("Remove"))
                        {
                            OptionQueue.Enqueue(MakePair(EArrayElementOption::Remove, Index));
                        }
                        else if (ImGui::Selectable("Duplicate"))
                        {
                            OptionQueue.Enqueue(MakePair(EArrayElementOption::Duplicate, Index));
                        }
                        ImGui::EndPopup();
                    }
                }

                TPair<EArrayElementOption, int32> Option;
                while (OptionQueue.Dequeue(Option))
                {
                    switch (Option.Key)
                    {
                    case EArrayElementOption::Insert:
                    {
                        Data->Insert(ElementType{}, Option.Value);
                        break;
                    }
                    case EArrayElementOption::Remove:
                    {
                        Data->RemoveAt(Option.Value);
                        break;
                    }
                    case EArrayElementOption::Duplicate:
                    {
                        Data->Insert((*Data)[Option.Value], Option.Value);
                    }
                    default:
                        break;
                    }
                }
            }
            ImGui::EndDisabled();
            ImGui::TreePop();
        }
    }
};

template <typename InMapType>
struct TMapProperty : public FProperty
{
    using KeyType = typename InMapType::KeyType;
    using ValueType = typename InMapType::ValueType;

    FProperty* KeyProperty = nullptr;
    FProperty* ValueProperty = nullptr;

    TMapProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Map, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override
    {
        FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

        enum class EMapElementOption : uint8
        {
            Remove,
        };

        TMap<KeyType, ValueType>* Data = static_cast<TMap<KeyType, ValueType>*>(DataPtr);

        if (ImGui::TreeNode(PropertyLabel))
        {
            ImGui::BeginDisabled(HasFlag(Flags, EPropertyFlags::VisibleAnywhere));
            {
                const ImGuiIO& IO = ImGui::GetIO();
                ImFont* IconFont = IO.Fonts->Fonts[1]; // FEATHER_FONT = 1

                ImGui::Text("Num of Elements: %d", Data->Num());

                ImGui::SameLine();
                ImGui::PushFont(IconFont);
                if (ImGui::Button("\ue9c8"))
                {
                    const KeyType DefaultKeyToAdd = KeyType{};
                    if (Data->Contains(DefaultKeyToAdd))
                    {
                        ImGui::OpenPopup("Duplicate Key Warning");
                    }
                    else
                    {
                        Data->Add(DefaultKeyToAdd, ValueType{});
                    }
                }
                ImGui::PopFont();
                ImGui::SetItemTooltip("Add Element");

                // --- 중복 키 경고 팝업 정의 ---
                // "Duplicate Key Warning" ID는 위 ImGui::OpenPopup 호출과 일치해야 함
                if (ImGui::BeginPopupModal("Duplicate Key Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("The key you are trying to add already exists in the map.\nPlease use a unique key.");
                    ImGui::Separator();

                    // 팝업을 닫는 OK 버튼
                    if (ImGui::Button("OK", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SetItemDefaultFocus(); // OK 버튼에 기본 포커스
                    ImGui::EndPopup();
                }
                // --- 중복 키 경고 팝업 정의 ---

                ImGui::SameLine();
                ImGui::PushFont(IconFont);
                if (ImGui::Button("\ue9f6"))
                {
                    Data->Empty();
                }
                ImGui::PopFont();
                ImGui::SetItemTooltip("Remove All Elements");

                TQueue<TPair<EMapElementOption, KeyType>> OptionQueue;
                for (int32 Idx = 0; auto& Pair : *Data)
                {
                    ImGui::PushID(&Pair);

                    const bool bIsOpen = ImGui::TreeNode(std::format("Elem [{}]", Idx).c_str());

                    std::string PopupLabel = std::format("MapElementOption##{}", Idx);
                    ImGui::SameLine();
                    if (ImGui::Button(std::format("...##{}", Idx).c_str()))
                    {
                        ImGui::OpenPopup(PopupLabel.c_str());
                    }

                    if (ImGui::BeginPopup(PopupLabel.c_str()))
                    {
                        if (ImGui::Selectable("Remove"))
                        {
                            OptionQueue.Enqueue(MakePair(EMapElementOption::Remove, Pair.Key));
                        }
                        ImGui::EndPopup();
                    }

                    if (bIsOpen)
                    {
                        constexpr std::string_view KeyTypeNameView = GetTypeName<KeyType>();
                        std::string KeyTypeName = std::string(KeyTypeNameView);
                        if (ImGui::TreeNode(std::format("Key ({})", KeyTypeName).c_str()))
                        {
                            // TODO: 겹치는 Key에 대해서는 나중에 수정해야함
                            KeyProperty->DisplayRawDataInImGui("", &const_cast<KeyType&>(Pair.Key));
                            ImGui::TreePop();
                        }

                        constexpr std::string_view ValueTypeNameView = GetTypeName<ValueType>();
                        std::string ValueTypeName = std::string(ValueTypeNameView);
                        if (ImGui::TreeNode(std::format("Value ({})", ValueTypeName).c_str()))
                        {
                            ValueProperty->DisplayRawDataInImGui("##Value", &Pair.Value);
                            ImGui::TreePop();
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                    ++Idx;
                }

                TPair<EMapElementOption, KeyType> Option;
                while (OptionQueue.Dequeue(Option))
                {
                    switch (Option.Key)
                    {
                    case EMapElementOption::Remove:
                    {
                        Data->Remove(Option.Value);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            ImGui::EndDisabled();
            ImGui::TreePop();
        }
    }
};

template <typename InSetType>
struct TSetProperty : public FProperty
{
    using ElementType = typename InSetType::ElementType;

    FProperty* ElementProperty = nullptr;

    TSetProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Set, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override
    {
        FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

        enum class ESetElementOption : uint8
        {
            Remove,
        };
    
        TSet<ElementType>* Data = static_cast<TSet<ElementType>*>(DataPtr);
    
        if (ImGui::TreeNode(PropertyLabel))
        {
            ImGui::BeginDisabled(HasFlag(Flags, EPropertyFlags::VisibleAnywhere));
            {
                const ImGuiIO& IO = ImGui::GetIO();
                ImFont* IconFont = IO.Fonts->Fonts[1]; // FEATHER_FONT = 1
    
                ImGui::Text("Num of Elements: %d", Data->Num());
    
                ImGui::SameLine();
                ImGui::PushFont(IconFont);
                if (ImGui::Button("\ue9c8"))
                {
                    const ElementType DefaultElementToAdd = ElementType{};
                    if (Data->Contains(DefaultElementToAdd))
                    {
                        ImGui::OpenPopup("Duplicate Element Warning");
                    }
                    else
                    {
                        Data->Add(DefaultElementToAdd);
                    }
                }
                ImGui::PopFont();
                ImGui::SetItemTooltip("Add Element");
    
                if (ImGui::BeginPopupModal("Duplicate Element Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("The element you are trying to add already exists in the set.\nPlease use a unique element.");
                    ImGui::Separator();
    
                    if (ImGui::Button("OK", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SetItemDefaultFocus();
                    ImGui::EndPopup();
                }
    
                ImGui::SameLine();
                ImGui::PushFont(IconFont);
                if (ImGui::Button("\ue9f6"))
                {
                    Data->Empty();
                }
                ImGui::PopFont();
                ImGui::SetItemTooltip("Remove All Elements");
    
                TQueue<TPair<ESetElementOption, ElementType>> OptionQueue;
                for (int32 Idx = 0; auto& Element : *Data)
                {
                    ImGui::PushID(&Element);

                    // TODO: 겹치는 Key에 대해서는 나중에 수정해야함
                    ElementProperty->DisplayRawDataInImGui(std::format("Elem [{}]", Idx).c_str(), &const_cast<ElementType&>(Element));

                    std::string PopupLabel = std::format("SetElementOption##{}", Idx);
                    ImGui::SameLine();
                    if (ImGui::Button(std::format("...##{}", Idx).c_str()))
                    {
                        ImGui::OpenPopup(PopupLabel.c_str());
                    }
    
                    if (ImGui::BeginPopup(PopupLabel.c_str()))
                    {
                        if (ImGui::Selectable("Remove"))
                        {
                            OptionQueue.Enqueue(MakePair(ESetElementOption::Remove, Element));
                        }
                        ImGui::EndPopup();
                    }
    
                    ImGui::PopID();
                    ++Idx;
                }
    
                TPair<ESetElementOption, ElementType> Option;
                while (OptionQueue.Dequeue(Option))
                {
                    switch (Option.Key)
                    {
                    case ESetElementOption::Remove:
                    {
                        Data->Remove(Option.Value);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            ImGui::EndDisabled();
            ImGui::TreePop();
        }
    }
};

template <typename InEnumType>
struct TEnumProperty : public FProperty
{
    using EnumType = InEnumType;

    TEnumProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Enum, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override
    {
        ImGui::BeginDisabled(HasFlag(Flags, EPropertyFlags::VisibleAnywhere));
        {
            FProperty::DisplayInImGui(Object);
        }
        ImGui::EndDisabled();
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override
    {
        FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

        EnumType* Data = static_cast<EnumType*>(DataPtr);
        constexpr auto EnumEntries = magic_enum::enum_entries<EnumType>();

        const std::string_view CurrentNameView = magic_enum::enum_name(*Data);
        const std::string CurrentName = std::string(CurrentNameView);

        ImGui::Text("%s", PropertyLabel);
        ImGui::SameLine();
        if (ImGui::BeginCombo(std::format("##{}", PropertyLabel).c_str(), CurrentName.c_str()))
        {
            for (const auto& [Enum, NameView] : EnumEntries)
            {
                const std::string EnumName = std::string(NameView);
                const bool bIsSelected = (*Data == Enum);
                if (ImGui::Selectable(EnumName.c_str(), bIsSelected))
                {
                    *Data = Enum;
                }
                if (bIsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
};

struct FSubclassOfProperty : public FProperty
{
    FSubclassOfProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::SubclassOf, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FObjectBaseProperty : public FProperty
{
    FObjectBaseProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        EPropertyType InType,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, InType, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const override;
};

struct FObjectProperty : public FObjectBaseProperty
{
    FObjectProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FObjectBaseProperty(InOwnerClass, InPropertyName, EPropertyType::Object, InSize, InOffset, InFlags)
    {
    }
};

struct FUnresolvedPtrProperty : public FObjectBaseProperty
{
    FUnresolvedPtrProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FObjectBaseProperty(InOwnerClass, InPropertyName, EPropertyType::UnresolvedPointer, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
    virtual void Resolve() override;
};

struct FStructProperty : public FProperty
{
    FStructProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Struct, InSize, InOffset, InFlags)
    {
    }
};


// struct FDelegateProperty : public FProperty {};  // TODO: 나중에 Delegate Property 만들기

// struct FMulticastDelegateProperty : public FProperty {};

namespace PropertyFactory::Private
{
template <typename T, EPropertyFlags InFlags>
FProperty* CreatePropertyForContainerType();

template <typename T, EPropertyFlags InFlags>
FProperty* MakeProperty(
    UClass* InOwnerClass,
    const char* InPropertyName,
    int32 InOffset
)
{
    // Flags 검사
    if constexpr (HasAllFlags<InFlags>(EPropertyFlags::EditAnywhere | EPropertyFlags::VisibleAnywhere))
    {
        // EditAnywhere와 VisibleAnywhere는 서로 같이 사용할 수 없음!!
        static_assert(TAlwaysFalse<T>, "EditAnywhere and VisibleAnywhere cannot be set at the same time.");
    }
    else if constexpr (HasAllFlags<InFlags>(EPropertyFlags::LuaReadOnly | EPropertyFlags::LuaReadWrite))
    {
        // LuaReadOnly와 LuaReadWrite는 서로 같이 사용할 수 없음!!
        static_assert(TAlwaysFalse<T>, "LuaReadOnly and LuaReadWrite cannot be set at the same time.");
    }

    // 각 타입에 맞는 Property 생성
    constexpr EPropertyType TypeEnum = GetPropertyType<T>();

    if constexpr      (TypeEnum == EPropertyType::Int8)        { return new FInt8Property        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Int16)       { return new FInt16Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Int32)       { return new FInt32Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Int64)       { return new FInt64Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt8)       { return new FUInt8Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt16)      { return new FUInt16Property      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt32)      { return new FUInt32Property      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt64)      { return new FUInt64Property      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Float)       { return new FFloatProperty       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Double)      { return new FDoubleProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Bool)        { return new FBoolProperty        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }

    else if constexpr (TypeEnum == EPropertyType::String)      { return new FStrProperty         { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Name)        { return new FNameProperty        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Vector2D)    { return new FVector2DProperty    { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Vector)      { return new FVectorProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Vector4)     { return new FVector4Property     { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Rotator)     { return new FRotatorProperty     { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Quat)        { return new FQuatProperty        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Transform)   { return new FTransformProperty   { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Matrix)      { return new FMatrixProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Color)       { return new FColorProperty       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::LinearColor) { return new FLinearColorProperty { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }

    else if constexpr (TypeEnum == EPropertyType::Array)
    {
        TArrayProperty<T>* Property = new TArrayProperty<T> { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };
        Property->ElementProperty = CreatePropertyForContainerType<typename T::ElementType, InFlags>();
        return Property;
    }
    else if constexpr (TypeEnum == EPropertyType::Map)
    {
        TMapProperty<T>* Property = new TMapProperty<T> { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };
        Property->KeyProperty = CreatePropertyForContainerType<typename T::KeyType, InFlags>();
        Property->ValueProperty = CreatePropertyForContainerType<typename T::ValueType, InFlags>();
        return Property;
    }
    else if constexpr (TypeEnum == EPropertyType::Set)
    {
        TSetProperty<T>* Property = new TSetProperty<T> { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };
        Property->ElementProperty = CreatePropertyForContainerType<typename T::ElementType, InFlags>();
        return Property;
    }

    else if constexpr (TypeEnum == EPropertyType::Enum)        { return new TEnumProperty<T>     { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Struct)      { return new FStructProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::SubclassOf)
    {
        if constexpr (std::derived_from<typename T::ElementType, UObject>)
        {
            FProperty* Property = new FSubclassOfProperty { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };
            Property->TypeSpecificData = T::ElementType::StaticClass();
            return Property;
        }
        else
        {
            // TSubclassOf에 UObject를 상속받은 클래스가 아닌 타입이 들어오면, 여기서 컴파일 에러가 날 수 있음
            static_assert(TAlwaysFalse<T>, "TSubclassOf template parameter must inherit from UObject");
        }
    }
    else if constexpr (TypeEnum == EPropertyType::Object)
    {
        using PointerType = std::remove_cvref_t<std::remove_pointer_t<T>>;
        FProperty* Property = new FObjectProperty { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };

        // Property륻 등록하는 시점에서는 아직 모든 UClass가 초기화되지 않았으므로, UEngine::Init()때 Property의 TypeName을 바탕으로 ClassMap에서 가져오기
        Property->TypeSpecificData = PointerType::StaticClass();
        return Property;
    }
    else if constexpr (TypeEnum == EPropertyType::UnresolvedPointer)
    {
        constexpr std::string_view TypeName = GetTypeName<T>();
        FProperty* Property = new FUnresolvedPtrProperty { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };
        Property->TypeSpecificData = FName(TypeName.data(), TypeName.size());
        return Property;
    }
    else if constexpr (TypeEnum == EPropertyType::Unknown)
    {
        static_assert(TAlwaysFalse<T>, "Unsupported Property Type"); // 지원되지 않는 타입!!
    }
    else
    {
        // 모든 Enum값에 대해서 처리하지 않으면 컴파일 에러
        static_assert(TAlwaysFalse<T>, "Not all property types are handled in MakeProperty function. Please add missing property type handling.");
    }

    std::unreachable(); // 이론상 도달할 수 없는 코드, (static_assert 지우면 호출됨)
}

template <typename T, EPropertyFlags InFlags>
FProperty* CreatePropertyForContainerType()
{
    constexpr EPropertyType TypeEnum = GetPropertyType<T>();

    if constexpr (
        TypeEnum == EPropertyType::Array
        || TypeEnum == EPropertyType::Set
        || TypeEnum == EPropertyType::Map
        || TypeEnum == EPropertyType::SubclassOf
    )
    {
        // 다차원 컨테이너는 UPROPERTY로 사용할 수 없음!!
        static_assert(TAlwaysFalse<T>, "Nested container types (e.g. TArray<TArray<T>>, TArray<TSet<T>>) cannot be used as UPROPERTY type.");
    }

    return MakeProperty<T, InFlags>(
        nullptr,
        "InnerProperty", // 실제 컨테이너 항목은 이 이름을 사용하지 않음
        0
    );
}
}
