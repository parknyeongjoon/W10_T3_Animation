#include "Shader.h"

#include "GraphicDevice.h"

bool FShader::IsOutDated()
{
    try
    {
        const std::filesystem::file_time_type CurrentLastWriteTime = std::filesystem::last_write_time(*FullPath);
        if (LastWriteTime < CurrentLastWriteTime)
        {
            LastWriteTime = CurrentLastWriteTime;
            return true;
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        // 파일 접근에 문제가 발생한 경우, 로깅 처리 후 false 또는 적절한 값을 반환합니다.
        // 예: std::cerr << "Error: " << e.what() << std::endl;
    }

    return false;
}

void FShader::Release()
{
    ShaderBlob->Release();
    ShaderBlob = nullptr;
}
