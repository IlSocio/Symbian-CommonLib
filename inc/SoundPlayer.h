
// LIBRARY mediaclientaudio.lib

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <MdaAudioSamplePlayer.h>

class MSoundPlayerObserver
	{
	public:
		virtual void SoundPlayedL(TInt aError) = 0;
	};


/**
 *  CSoundPlayer
 * 
 */
class CSoundPlayer : public CBase, public MMdaAudioPlayerCallback
{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CSoundPlayer();

	/**
	 * Two-phased constructor.
	 */
	static CSoundPlayer* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CSoundPlayer* NewLC();

public:
	void PlaySoundL(const TDesC& aFilename, MSoundPlayerObserver* aObserver=NULL);

protected:
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);


private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CSoundPlayer();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	CMdaAudioPlayerUtility* iAudioPlayer;
	MSoundPlayerObserver* iObserver;
};

#endif // SOUNDPLAYER_H

