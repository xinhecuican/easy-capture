#include "dxgimanager.h"
#include <QDebug>
#include <QGuiApplication>
#include <QCursor>

Texture::~Texture()
{
    m_texture->Release();
}

DxgiTextureStaging::DxgiTextureStaging(ID3D11Device *device, ID3D11DeviceContext *context)
    : m_device(device), m_context(context)
{

}

DxgiTextureStaging::~DxgiTextureStaging()
{
    m_device->Release();
    m_context->Release();
}

QPixmap DxgiTextureStaging::copyToImage(IDXGIResource *res)
{
    D3D11_TEXTURE2D_DESC desc;
    ID3D11Texture2D *textrueRes = nullptr;
    HRESULT hr = res->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&textrueRes));
    if (FAILED(hr)) {
        qWarning() << "Failed to ID3D11Texture2D result =" << hex << uint(hr);
        return QPixmap();
    }
    textrueRes->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = desc.Width;
    texDesc.Height = desc.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.Format = desc.Format;
    texDesc.BindFlags = 0;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    texDesc.MiscFlags = 0;
    m_device->CreateTexture2D(&texDesc, nullptr, &m_texture);
    m_context->CopyResource(m_texture, textrueRes);

    IDXGISurface1 *surface = nullptr;
    hr = m_texture->QueryInterface(__uuidof(IDXGISurface1), reinterpret_cast<void **>(&surface));
    if (FAILED(hr)) {
        qWarning() << "Failed to QueryInterface IDXGISurface1 ErrorCode =" << hex << uint(hr);
        return QPixmap();
    }

    DXGI_MAPPED_RECT map;
    surface->Map(&map, DXGI_MAP_READ);
    QPixmap pixmap = QPixmap::fromImage(QImage(static_cast<uchar *>(map.pBits),
                                               int(desc.Width), int(desc.Height), QImage::Format_ARGB32));
    surface->Unmap();
    surface->Release();
    m_texture->Release();

    return pixmap;
}

DxgiManager::DxgiManager()
{
    IDXGIFactory1* pFactory1;

    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory1));

    if (FAILED(hr)) {
        deviceSuccess = false;
        return;
    }

    for (UINT i = 0;; i++){
        IDXGIAdapter1* pAdapter1 = nullptr;
        hr = pFactory1->EnumAdapters1(i, &pAdapter1);
        if (hr == DXGI_ERROR_NOT_FOUND){
            break;
        }
        if (FAILED(hr)) {
            deviceSuccess = false;
            return;
        }
        for (UINT j = 0;; j++){
            IDXGIOutput *pOutput = nullptr;
            HRESULT hr = pAdapter1->EnumOutputs(j, &pOutput);
            if (hr == DXGI_ERROR_NOT_FOUND){
                break;
            }
            DXGI_OUTPUT_DESC desc;
            hr = pOutput->GetDesc(&desc);
            if(FAILED(hr)) {
                deviceSuccess = false;
                return;
            }
            DxgiData data;
            data.pAdapter = pAdapter1;
            data.pOutput = pOutput;
            int left = desc.DesktopCoordinates.left;
            int top = desc.DesktopCoordinates.top;
            int width = desc.DesktopCoordinates.right - left;
            int height = desc.DesktopCoordinates.bottom - top;
            data.bound = QRect(left, top, width, height);
//            qDebug() << QString::fromWCharArray(desc.DeviceName) << data.bound;
            dxgiMonitors.append(data);
        }
    }
}

DxgiManager::~DxgiManager()
{
}

bool DxgiManager::init(int initIndex)
{
    if(!deviceSuccess) return false;
    int index = initIndex;
    if(initIndex == -1) index = getCurrentIndex();

    IDXGIOutput *dxgiOutput = nullptr;
    HRESULT hr;
    if (dxgiMonitors.size() > 0 && index < dxgiMonitors.size()) {
        if(dxgiMonitors.at(index).init) {
            currentIndex = index;
            return true;
        }
        dxgiOutput = dxgiMonitors.at(index).pOutput;
    }
    else {
        qWarning() << "Failed to IDXGIOutput is Empty!";
        return false;
    }

    ID3D11Device *d3dDevice = nullptr;
    ID3D11DeviceContext *d3dContext = nullptr;
    // Driver types supported
    D3D_DRIVER_TYPE DriverTypes[] ={
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT NumDriverTypes = ARRAYSIZE(DriverTypes);
    // Feature levels supported
    D3D_FEATURE_LEVEL FeatureLevels[] ={
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);


    for (int i = 0; i < NumFeatureLevels; i++)
    {
        D3D_FEATURE_LEVEL FeatureLevel = FeatureLevels[i];
        hr = D3D11CreateDevice(dxgiMonitors.at(index).pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, 0, 0, D3D11_SDK_VERSION, &d3dDevice, &FeatureLevel, &d3dContext);
        if (SUCCEEDED(hr))
        {
            break;
        }
    }
    if (FAILED(hr)) {
        qWarning() << "Failed to D3D11CreateDevice ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIDevice *dxgiDevice = nullptr;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if(FAILED(hr)) {
        qWarning() << "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIAdapter *dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    dxgiDevice->Release();
    if (FAILED(hr)) {
        qWarning() << "Failed to Get IDXGIAdapter ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIOutput6 *dxgiOutput6 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void**>(&dxgiOutput6));
    dxgiOutput->Release();
    if (FAILED(hr)) {
        qWarning() << "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }
    IDXGIOutputDuplication *_pDXGIOutputDup = nullptr;
    hr = dxgiOutput6->DuplicateOutput(d3dDevice, &_pDXGIOutputDup);
    dxgiOutput6->Release();
    if (FAILED(hr)) {
        qWarning() << "Failed to DuplicateOutput ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }
    currentIndex = index;
    dxgiMonitors[index].duplication = _pDXGIOutputDup;
    dxgiMonitors[index].texture = new DxgiTextureStaging(d3dDevice, d3dContext);
    dxgiMonitors[index].init = true;
    return true;
}

QString DxgiManager::lastError() const
{
    return m_lastError;
}

QPixmap DxgiManager::grabScreen(bool* success)
{
    *success = init();
    if((*success) == false){
        return QPixmap();
    }
    IDXGIResource *desktopRes;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    while (true) {
        HRESULT hr = dxgiMonitors[currentIndex].duplication->AcquireNextFrame(100, &frameInfo, &desktopRes);
        if (FAILED(hr)) {
            if(hr == DXGI_ERROR_INVALID_CALL){
                dxgiMonitors[currentIndex].duplication->ReleaseFrame();
            }
            qWarning() << "Failed to AcquireNextFrame ErrorCode = " + QString::number(uint(hr), 16);
            *success = false;
            return QPixmap();
        }

        if (frameInfo.LastPresentTime.QuadPart) break;

        dxgiMonitors[currentIndex].duplication->ReleaseFrame();
    }
    QPixmap pix = dxgiMonitors[currentIndex].texture->copyToImage(desktopRes);
    dxgiMonitors[currentIndex].duplication->ReleaseFrame();
    *success = true;
    return pix;
}

QPixmap DxgiManager::grabScreen(int index, bool* success){
    *success = init(index);
    if((*success) == false){
        return QPixmap();
    }
    IDXGIResource *desktopRes;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    while (true) {
        HRESULT hr = dxgiMonitors[currentIndex].duplication->AcquireNextFrame(100, &frameInfo, &desktopRes);
        if (FAILED(hr)) {
            if(hr == DXGI_ERROR_INVALID_CALL){
                dxgiMonitors[currentIndex].duplication->ReleaseFrame();
            }
            qWarning() << "Failed to AcquireNextFrame ErrorCode = " + QString::number(uint(hr), 16);
            *success = false;
            return QPixmap();
        }

        if (frameInfo.LastPresentTime.QuadPart){
            break;
        }
        dxgiMonitors[currentIndex].duplication->ReleaseFrame();
    }

    QPixmap pix = dxgiMonitors[currentIndex].texture->copyToImage(desktopRes);
    dxgiMonitors[currentIndex].duplication->ReleaseFrame();
    *success = true;
    return pix;
}

int DxgiManager::getCurrentIndex(){
    QPoint pos = QCursor::pos();
    int index = 0;
    for(auto monitor : dxgiMonitors){
        if(monitor.bound.contains(pos)){
            return index;
        }
        index++;
    }
    return 0;
}
