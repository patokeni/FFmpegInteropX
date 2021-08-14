#pragma once

using namespace Windows::Foundation;
using namespace Windows::Globalization;

namespace FFmpegInterop {
	ref class FFmpegInteropMSS;
	ref class InterruptCallbackArgs;
	
	public delegate void InterruptCallback(FFmpegInteropMSS^ mss, InterruptCallbackArgs^ args);

	public ref class InterruptCallbackArgs sealed {
	public:
		property DateTime PreviousVideoSampleTime {
			DateTime get() {
				return previousVideoSampleTime;
			}
		}

		property DateTime PreviousAudioSampleTime {
			DateTime get() {
				return previousAudioSampleTime;
			}
		}

		void Interrupt() {
			ShouldInterrupt = true;
		}

	internal:
		InterruptCallbackArgs(DateTime previousVideoSampleTime, DateTime previousAudioSampleTime) {
			this->previousVideoSampleTime = previousVideoSampleTime;
			this->previousAudioSampleTime = previousAudioSampleTime;
		}

		bool ShouldInterrupt;

	private:
		DateTime previousVideoSampleTime;
		DateTime previousAudioSampleTime;
	};

	static DateTime GetCurrentDateTime() {
		Calendar^ cal = ref new Calendar();
		return cal->GetDateTime();
	}
}