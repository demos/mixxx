#ifndef MIXXX_SOUNDSOURCESNDFILE_H
#define MIXXX_SOUNDSOURCESNDFILE_H

#include "sources/soundsourceprovider.h"

#ifdef Q_OS_WIN
//Enable unicode in libsndfile on Windows
//(sf_open uses UTF-8 otherwise)
#include <windows.h>
#define ENABLE_SNDFILE_WINDOWS_PROTOTYPES 1
#endif
#include <sndfile.h>

namespace mixxx {

class SoundSourceSndFile: public mixxx::SoundSource {
public:
    explicit SoundSourceSndFile(const QUrl& url);
    ~SoundSourceSndFile() override;

    void close() override;

    SINT seekSampleFrame(SINT frameIndex) override;

    SINT readSampleFrames(SINT numberOfFrames,
            CSAMPLE* sampleBuffer) override;

private:
    OpenResult tryOpen(const AudioSourceConfig& audioSrcCfg) override;

    SNDFILE* m_pSndFile;
};

class SoundSourceProviderSndFile: public SoundSourceProvider {
public:
    QString getName() const override;

    SoundSourceProviderPriority getPriorityHint(
            const QString& supportedFileExtension) const override {
        Q_UNUSED(supportedFileExtension);
        // libsnd will be used as a fallback
        return SoundSourceProviderPriority::LOWER;
    }

    QStringList getSupportedFileExtensions() const override;

    SoundSourcePointer newSoundSource(const QUrl& url) override {
        return SoundSourcePointer(new SoundSourceSndFile(url));
    }
};

} // namespace mixxx

#endif // MIXXX_SOUNDSOURCESNDFILE_H
