#include "SoundPlayer.h"

CSoundPlayer::CSoundPlayer()
{
	// No implementation required
}

CSoundPlayer::~CSoundPlayer()
{
	//
	if (iAudioPlayer)
		{
		iAudioPlayer->Stop();
		iAudioPlayer->Close();
		}
	delete iAudioPlayer;
	iAudioPlayer = NULL;
}

CSoundPlayer* CSoundPlayer::NewLC()
{
	CSoundPlayer* self = new (ELeave)CSoundPlayer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSoundPlayer* CSoundPlayer::NewL()
{
	CSoundPlayer* self=CSoundPlayer::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CSoundPlayer::ConstructL()
{
	iAudioPlayer = CMdaAudioPlayerUtility::NewL(*this);
}

void CSoundPlayer::PlaySoundL(const TDesC& aFilename, MSoundPlayerObserver* aObserver)
{
	iObserver = aObserver;
	iAudioPlayer->Stop();
	iAudioPlayer->Close();
	iAudioPlayer->OpenFileL(aFilename);	
}

void CSoundPlayer::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
	if (aError != KErrNone)
	{	
		if (iObserver)
			TRAP_IGNORE( iObserver->SoundPlayedL(aError) );
		return;
	}
	iAudioPlayer->SetVolume(iAudioPlayer->MaxVolume());
	iAudioPlayer->Play(); // action!
}

void CSoundPlayer::MapcPlayComplete(TInt aError)
{
	// ... do something ...
	iAudioPlayer->Close();
	if (iObserver)
		TRAP_IGNORE( iObserver->SoundPlayedL(aError) );
}
