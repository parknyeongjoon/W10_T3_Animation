#include "Property.h"

#include "Class.h"
#include "ScriptStruct.h"
#include "UObjectHash.h"
#include "Editor/UnrealEd/ImGuiWidget.h"
#include "Math/NumericLimits.h"
#include "Template/SubclassOf.h"
#include "CoreUObject/UObject/ObjectUtils.h"

#include "ImGui/imgui.h"
#include "Serialization/Archive2.h"
#include "Serialization/Serializer.h"

template <typename Type, typename... Types>
concept TIsAnyOf = (std::same_as<Type, Types> || ...);

template <typename T>
concept NumericType = TIsAnyOf<T, int8, int16, int32, int64, uint8, uint16, uint32, uint64, float, double>;

static constexpr int32 IMGUI_FSTRING_BUFFER_SIZE = 2048;


struct FPropertyUIHelper
{
    template <NumericType NumType>
    static void DisplayNumericDragN(const char* PropertyLabel, void* InData, int Components, float Speed = 1.0f, const char* Format = nullptr)
    {
        NumType* Data = static_cast<NumType*>(InData);
        constexpr NumType Min = TNumericLimits<NumType>::Lowest();
        constexpr NumType Max = TNumericLimits<NumType>::Max();

        ImGuiDataType DataType;
        if constexpr (std::same_as<NumType, int8>) { DataType = ImGuiDataType_S8; }
        else if constexpr (std::same_as<NumType, int16>) { DataType = ImGuiDataType_S16; }
        else if constexpr (std::same_as<NumType, int32>) { DataType = ImGuiDataType_S32; }
        else if constexpr (std::same_as<NumType, int64>) { DataType = ImGuiDataType_S64; }
        else if constexpr (std::same_as<NumType, uint8>) { DataType = ImGuiDataType_U8; }
        else if constexpr (std::same_as<NumType, uint16>) { DataType = ImGuiDataType_U16; }
        else if constexpr (std::same_as<NumType, uint32>) { DataType = ImGuiDataType_U32; }
        else if constexpr (std::same_as<NumType, uint64>) { DataType = ImGuiDataType_U64; }
        else if constexpr (std::same_as<NumType, float>) { DataType = ImGuiDataType_Float; }
        else if constexpr (std::same_as<NumType, double>) { DataType = ImGuiDataType_Double; }
        else { static_assert(TAlwaysFalse<NumType>); }

        ImGui::Text("%s", PropertyLabel);
        ImGui::SameLine();
        ImGui::DragScalarN(std::format("##{}", PropertyLabel).c_str(), DataType, Data, Components, Speed, &Min, &Max, Format);
    }
};


void FProperty::DisplayInImGui(UObject* Object) const
{
    if (!HasAnyFlags(Flags, EPropertyFlags::EditAnywhere | EPropertyFlags::VisibleAnywhere))
    {
        return;
    }

    void* Data = GetPropertyData(Object);
    DisplayRawDataInImGui(Name, Data);
}

void FProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
}

void FNumericProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FInt8Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int8>(PropertyLabel, DataPtr, 1);
}

void FInt16Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int16>(PropertyLabel, DataPtr, 1);
}

void FInt32Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int32>(PropertyLabel, DataPtr, 1);
}

void FInt64Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int64>(PropertyLabel, DataPtr, 1);
}

void FUInt8Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint8>(PropertyLabel, DataPtr, 1);
}

void FUInt16Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint16>(PropertyLabel, DataPtr, 1);
}

void FUInt32Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint32>(PropertyLabel, DataPtr, 1);
}

void FUInt64Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint64>(PropertyLabel, DataPtr, 1);
}

void FFloatProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 1);
}

void FDoubleProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<double>(PropertyLabel, DataPtr, 1);
}

void FBoolProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FBoolProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    ImGui::Checkbox(PropertyLabel, static_cast<bool*>(DataPtr));
}

void FStrProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FStrProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FString* Data = static_cast<FString*>(DataPtr);

    char Buffer[IMGUI_FSTRING_BUFFER_SIZE];
    FCStringAnsi::Strncpy(Buffer, Data->ToAnsiString().c_str(), IMGUI_FSTRING_BUFFER_SIZE);
    Buffer[IMGUI_FSTRING_BUFFER_SIZE - 1] = '\0'; // 항상 널 종료 보장

    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    if (ImGui::InputText(std::format("##{}", PropertyLabel).c_str(), Buffer, IMGUI_FSTRING_BUFFER_SIZE))
    {
        *Data = Buffer;
    }
}

void FStrProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    FString& Str = *reinterpret_cast<FString*>(DataPtr);
    if (Ar.IsSaving())
    {
        int32 Len = Str.Len();
        Ar.SerializeRaw(&Len, sizeof(Len));
        Ar.SerializeRaw(const_cast<TCHAR*>(*Str), Len  * sizeof(TCHAR));
    }
    else
    {
        int32 Len = 0;
        Ar.SerializeRaw(&Len, sizeof(Len));
        TArray<TCHAR> Buffer;
        Buffer.SetNum(Len + 1);
        Ar.SerializeRaw(Buffer.GetData(), Len * sizeof(TCHAR));
        Str = FString(Buffer.GetData());
    }
}

void FNameProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    const FName* Data = static_cast<FName*>(DataPtr);
    std::string NameStr = Data->ToString().ToAnsiString();

    // ReadOnly Mode
    ImGui::BeginDisabled(true);
    {
        ImGui::Text("%s", PropertyLabel);
        ImGui::SameLine();
        ImGui::InputText(std::format("##{}", PropertyLabel).c_str(), NameStr.data(), NameStr.size(), ImGuiInputTextFlags_ReadOnly);
    }
    ImGui::EndDisabled();
}

void FNameProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr : 이미 Offset이 적용된 FName 위치
    FName& NameVal = *reinterpret_cast<FName*>(DataPtr);

    if (Ar.IsSaving())
    {
        // 1) FString 으로 변환
        FString Str = NameVal.ToString();
        int32 Len = Str.Len();

        // 2) 길이 저장
        Ar.SerializeRaw(&Len, sizeof(Len));
        // 3) 문자열(TCHAR) 저장 (널종료 포함)
        Ar.SerializeRaw(const_cast<TCHAR*>(*Str), Len * sizeof(TCHAR));
    }
    else // Loading
    {
        // 1) 저장된 길이 읽기
        int32 Len = 0;
        Ar.SerializeRaw(&Len, sizeof(Len));

        // 2) 버퍼 할당 후 문자열 읽기
        TArray<TCHAR> Buffer;
        Buffer.SetNum(Len + 1);
        Ar.SerializeRaw(Buffer.GetData(), Len  * sizeof(TCHAR));

        // 3) FString → FName 으로 복원
        FString Str(Buffer.GetData());
        NameVal = FName(Str);
    }
}

void FVector2DProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FVector2DProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 2);
}

void FVector2DProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FVector2D* Val = static_cast<FVector2D*>(DataPtr);
    // 순서대로 직렬화합니다
    Ar.SerializeRaw(&Val->X, sizeof(Val->X));
    Ar.SerializeRaw(&Val->Y, sizeof(Val->Y));
}

void FVectorProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FVectorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 3);
}

void FVectorProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FVector& Val = *reinterpret_cast<FVector*>(DataPtr);

    Ar.SerializeRaw(&Val.X, sizeof(Val.X));
    Ar.SerializeRaw(&Val.Y, sizeof(Val.Y));
    Ar.SerializeRaw(&Val.Z, sizeof(Val.Z));
}

void FVector4Property::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FVector4Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 4);
}

void FVector4Property::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FVector4* Val = static_cast<FVector4*>(DataPtr);
    // 순서대로 직렬화합니다
    Ar.SerializeRaw(&Val->X, sizeof(Val->X));
    Ar.SerializeRaw(&Val->Y, sizeof(Val->Y));
    Ar.SerializeRaw(&Val->Z, sizeof(Val->Z));
    Ar.SerializeRaw(&Val->W, sizeof(Val->W));
}

void FRotatorProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FRotatorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 3);
}

void FRotatorProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FRotator* Val = static_cast<FRotator*>(DataPtr);
    // 순서대로 직렬화합니다
    Ar.SerializeRaw(&Val->Pitch, sizeof(Val->Pitch));
    Ar.SerializeRaw(&Val->Yaw, sizeof(Val->Yaw));
    Ar.SerializeRaw(&Val->Roll, sizeof(Val->Roll));
}

void FQuatProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FQuatProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 4);
}

void FQuatProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 FVector2D 시작 주소를 가리킵니다
    FQuat& Val = *reinterpret_cast<FQuat*>(DataPtr);
    // X, Y를 순서대로 직렬화합니다
    Ar.SerializeRaw(&Val.W, sizeof(Val.W));
    Ar.SerializeRaw(&Val.X, sizeof(Val.X));
    Ar.SerializeRaw(&Val.Y, sizeof(Val.Y));
    Ar.SerializeRaw(&Val.Z, sizeof(Val.Z));
}

void FTransformProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    if (ImGui::TreeNode(PropertyLabel))
    {
        ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
        {
            FTransform* Data = static_cast<FTransform*>(DataPtr);
            FRotator Rotation = Data->Rotator();

            FImGuiWidget::DrawVec3Control("Location", Data->Location);
            FImGuiWidget::DrawRot3Control("Rotation", Rotation);
            FImGuiWidget::DrawVec3Control("Scale", Data->Scale, 1.0f);

            Data->Rotation = Rotation.ToQuaternion();
        }
        ImGui::EndDisabled();
        ImGui::TreePop();
    }
}

void FTransformProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FTransform* Val = static_cast<FTransform*>(DataPtr);
    // 순서대로 직렬화합니다
    Ar.SerializeRaw(&Val->Rotation.W, sizeof(Val->Rotation.W));
    Ar.SerializeRaw(&Val->Rotation.X, sizeof(Val->Rotation.X));
    Ar.SerializeRaw(&Val->Rotation.Y, sizeof(Val->Rotation.Y));
    Ar.SerializeRaw(&Val->Rotation.Z, sizeof(Val->Rotation.Z));
    
    Ar.SerializeRaw(&Val->Location.X, sizeof(Val->Location.X));
    Ar.SerializeRaw(&Val->Location.Y, sizeof(Val->Location.Y));
    Ar.SerializeRaw(&Val->Location.Z, sizeof(Val->Location.Z));

    Ar.SerializeRaw(&Val->Scale.X, sizeof(Val->Scale.X));
    Ar.SerializeRaw(&Val->Scale.Y, sizeof(Val->Scale.Y));
    Ar.SerializeRaw(&Val->Scale.Z, sizeof(Val->Scale.Z));
}

void FMatrixProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    // TODO: 짐벌락 현상 있음
    if (ImGui::TreeNode(PropertyLabel))
    {
        bool bChanged = false;
        FMatrix* Data = static_cast<FMatrix*>(DataPtr);

        ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
        {
            FTransform Transform = FTransform(*Data);
            FRotator Rotation = Transform.Rotator();

            bChanged |= FImGuiWidget::DrawVec3Control("Location", Transform.Location);
            bChanged |= FImGuiWidget::DrawRot3Control("Rotation", Rotation);
            bChanged |= FImGuiWidget::DrawVec3Control("Scale", Transform.Scale, 1.0f);

            if (bChanged)
            {
                *Data =
                    FMatrix::CreateScaleMatrix(Transform.Scale)
                    * FMatrix::CreateRotationMatrix(Rotation.ToQuaternion())
                    * FMatrix::CreateTranslationMatrix(Transform.Location);
            }
        }
        ImGui::EndDisabled();

        if (ImGui::TreeNode("Advanced"))
        {
            ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
            {
                ImGui::DragFloat4("##1", Data->M[0], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
                ImGui::DragFloat4("##2", Data->M[1], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
                ImGui::DragFloat4("##3", Data->M[2], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
                ImGui::DragFloat4("##4", Data->M[3], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
            }
            ImGui::EndDisabled();
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

void FMatrixProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FMatrix* Val = static_cast<FMatrix*>(DataPtr);
    // 순서대로 직렬화합니다
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            Ar.SerializeRaw(&Val->M[row][col], sizeof(Val->M[row][col]));
        }
    }
}

void FColorProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FColorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FColor* Data = static_cast<FColor*>(DataPtr);
    FLinearColor LinearColor = FLinearColor(*Data);

    constexpr ImGuiColorEditFlags Flags =
        ImGuiColorEditFlags_DisplayRGB
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_AlphaPreview
        | ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    if (ImGui::ColorEdit4(std::format("##{}", PropertyLabel).c_str(), reinterpret_cast<float*>(&LinearColor), Flags))
    {
        *Data = LinearColor.ToColorRawRGB8();
    }
}

void FColorProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FColor* Val = static_cast<FColor*>(DataPtr);
    // Bits 멤버(ARGB 4바이트) 전체를 일괄 직렬화
    Ar.SerializeRaw(&Val->Bits, sizeof(Val->Bits));
}

void FLinearColorProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FLinearColorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FLinearColor* Data = static_cast<FLinearColor*>(DataPtr);

    constexpr ImGuiColorEditFlags Flags =
        ImGuiColorEditFlags_Float
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_AlphaPreview
        | ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    ImGui::ColorEdit4(std::format("##{}", PropertyLabel).c_str(), reinterpret_cast<float*>(Data), Flags);
}

void FLinearColorProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // DataPtr는 시작 주소를 가리킵니다
    FLinearColor* Val = static_cast<FLinearColor*>(DataPtr);

    Ar.SerializeRaw(&Val->RGBA[0], sizeof(Val->RGBA[0]));
    Ar.SerializeRaw(&Val->RGBA[1], sizeof(Val->RGBA[1]));
    Ar.SerializeRaw(&Val->RGBA[2], sizeof(Val->RGBA[2]));
    Ar.SerializeRaw(&Val->RGBA[3], sizeof(Val->RGBA[3]));
}

void FSubclassOfProperty::DisplayInImGui(UObject* Object) const
{
    ImGui::BeginDisabled(HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere));
    {
        FProperty::DisplayInImGui(Object);
    }
    ImGui::EndDisabled();
}

void FSubclassOfProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    TSubclassOf<UObject>* Data = static_cast<TSubclassOf<UObject>*>(DataPtr);
    UClass* CurrentClass = GetSpecificClass();
    if (CurrentClass == nullptr)
    {
        return;
    }

    TArray<UClass*> ChildClasses;
    GetChildOfClass(CurrentClass, ChildClasses);

    const std::string CurrentClassName = (*Data) ? (*Data)->GetName().ToAnsiString() : "None";
    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    if (ImGui::BeginCombo(std::format("##{}", PropertyLabel).c_str(), CurrentClassName.c_str()))
    {
        if (ImGui::Selectable("None", !(*Data)))
        {
            *Data = nullptr;
        }

        for (UClass* ChildClass : ChildClasses)
        {
            const std::string ChildClassName = ChildClass->GetName().ToAnsiString();
            const bool bIsSelected = (*Data) && (*Data) == ChildClass;
            if (ImGui::Selectable(ChildClassName.c_str(), bIsSelected))
            {
                *Data = ChildClass;
            }
            if (bIsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void FSubclassOfProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    // UClass* 포인터 직렬화: 널 플래그 + 클래스 이름 저장/복원
    UClass*& ClassRef = *reinterpret_cast<UClass**>(reinterpret_cast<std::byte*>(DataPtr) + Offset);
    if (Ar.IsSaving())
    {
        bool bHas = (ClassRef != nullptr);
        Ar.SerializeRaw(&bHas, sizeof(bHas));
        if (bHas)
        {
            FString Name = ClassRef->GetName();
            int32 Len = Name.Len();
            Ar.SerializeRaw(&Len, sizeof(Len));
            Ar.SerializeRaw(const_cast<TCHAR*>(*Name), (Len + 1) * sizeof(TCHAR));
        }
    }
    else
    {
        bool bHas = false;
        Ar.SerializeRaw(&bHas, sizeof(bHas));
        if (bHas)
        {
            int32 Len = 0;
            Ar.SerializeRaw(&Len, sizeof(Len));
            TArray<TCHAR> Buffer;
            Buffer.SetNum(Len + 1);
            Ar.SerializeRaw(Buffer.GetData(), (Len + 1) * sizeof(TCHAR));
            FString Name(Buffer.GetData());
            ClassRef = UClass::FindClass(FName(Name));
        }
        else
        {
            ClassRef = nullptr;
        }
    }
}

void FObjectProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    if (ImGui::TreeNodeEx(PropertyLabel, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
        UObject** Object = static_cast<UObject**>(DataPtr);

        if (const UClass* ObjectClass = IsValid(*Object) ? (*Object)->GetClass() : nullptr)
        {
            // 포인터가 가리키는 객체를 수정, 여기서는 UObject의 인스턴스
            if (HasAnyFlags(Flags, EPropertyFlags::EditAnywhere))
            {
                TArray<UObject*> ChildObjects;
                GetObjectsOfClass(ObjectClass, ChildObjects, true);

                if (ImGui::BeginCombo(std::format("##{}", PropertyLabel).c_str(), (*Object)->GetName().ToAnsiString().c_str()))
                {
                    for (UObject* ChildObject : ChildObjects)
                    {
                        const std::string ObjectName = ChildObject->GetName().ToAnsiString();
                        const bool bIsSelected = ChildObject == *Object;
                        if (ImGui::Selectable(ObjectName.c_str(), bIsSelected))
                        {
                            // TODO: 나중에 수정, 지금은 목록만 보여주고, 설정은 안함
                            *Object = ChildObject;
                        }
                        if (bIsSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                        ImGui::Separator();
                    }
                    ImGui::EndCombo();
                }
            }
            else if (HasAnyFlags(Flags, EPropertyFlags::VisibleAnywhere))
            {
                const UClass* ChildClass = ObjectClass;
                for (; ChildClass; ChildClass = ChildClass->GetSuperClass())
                {
                    for (const FProperty* Prop : ChildClass->GetProperties())
                    {
                        Prop->DisplayInImGui(*Object);
                    }
                }
            }
        }
        ImGui::TreePop();
    }
}

void FObjectProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    UObject*& Ref = *reinterpret_cast<UObject**>(DataPtr);
    if (Ar.IsSaving())
    {
        bool bHas = Ref != nullptr;
        Ar.SerializeRaw(&bHas, sizeof(bHas));
        if (bHas)
        {
            Serializer::Save(Ar, Ref);
        }
    }
    else
    {
        bool bHas = false;
        Ar.SerializeRaw(&bHas, sizeof(bHas));
        if (bHas)
        {
            Ref = Serializer::Load(Ar);
        }
        else
        {
            Ref = nullptr;
        }
    }
}

void FStructProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    if (UScriptStruct* const* StructType = std::get_if<UScriptStruct*>(&TypeSpecificData))
    {
        if (ImGui::TreeNodeEx(PropertyLabel, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (const FProperty* Property : (*StructType)->GetProperties())
            {
                void* Data = static_cast<std::byte*>(DataPtr) + Property->Offset;
                Property->DisplayRawDataInImGui(Property->Name, Data);
            }
            ImGui::TreePop();
        }
    }
}

void FStructProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    if (UScriptStruct* const* StructType = std::get_if<UScriptStruct*>(&TypeSpecificData))
    {
        for (const FProperty* Property : (*StructType)->GetProperties())
        {
            void* Data = static_cast<std::byte*>(DataPtr) + Property->Offset;
            Property->Serialize(Ar, Data);
        }
    }
}

void FUnresolvedPtrProperty::DisplayInImGui(UObject* Object) const
{
    if (Type == EPropertyType::Unknown)
    {
        return;
    }
    ResolvedProperty->DisplayInImGui(Object);
}


void FUnresolvedPtrProperty::Serialize(FArchive2& Ar, void* DataPtr) const
{
    if (Type == EPropertyType::Unknown)
    {
        return;
    }

    void* Data = static_cast<std::byte*>(DataPtr);
    ResolvedProperty->Serialize(Ar, Data);
}