#include "audiodevice.hpp"

AudioDevice::AudioDevice(QObject *parent) : QIODevice(parent), spu(nullptr)
{

}

qint64 AudioDevice::readData(char *data, qint64 maxlen)
{
    if (spu->get_samples())
    {
        for (int i = 0; i < 1024; i += 2)
        {
            *(int16_t*)&data[i] = i;
            *(int16_t*)&data[i + 1] = i;
        }
        return 1024;
    }
    return 0;
    //return spu->output_buffer((int16_t*)data) * 2;
}

qint64 AudioDevice::writeData(const char *data, qint64 len)
{
    return 0;
}

qint64 AudioDevice::bytesAvailable() const
{
    return spu->get_samples() * 2 + QIODevice::bytesAvailable();
}

void AudioDevice::set_SPU(SPU *spu)
{
    this->spu = spu;
}
