/***************************************************************************************************
    Our audio elements
***************************************************************************************************/

class AudioElement;

class AudioMaster
{
private:
    int m_samplerate;
    std::vector<AudioElement> m_elements;

public:
    int getSampleRate();

};

class AudioElement
{
public:
    AudioMaster *getAudioMaster();
};


class AudioStream : public AudioElement
{
public:
    double getSample();
    // double getCachedSample();   // Calls getNextSample if not yet calculated
};

