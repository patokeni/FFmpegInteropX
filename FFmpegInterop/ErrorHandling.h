#pragma once

#include <stdlib.h>

extern "C"
{
#include <libavutil/error.h>
}

using namespace Platform;
using namespace Windows::Foundation;

#define FI_INITERRCTX() (errorContext = ref new ErrorContext())
#define FI_REPORTERR(expression, message) (errorContext->ReportError((expression), (message)))

namespace FFmpegInterop {
	ref class FFmpegInteropMSS;
	ref class ErrorContext;
	ref class AVErrorEventArgs;
	ref class AVErrorHelper;

	public delegate void AVErrorEventHandler(FFmpegInteropMSS^ mss, AVErrorEventArgs^ args);

	public ref class AVErrorHelper sealed {
	public:
		static String^ GetErrorString(int error) {
			char* buf = new char[AV_ERROR_MAX_STRING_SIZE];
			int ret = av_strerror(error, buf, AV_ERROR_MAX_STRING_SIZE);

			if (ret != 0) {
				delete buf;
				return "";
			}

			size_t len = strlen(buf) + 1;
			size_t msglen = 0;

			wchar_t* msg;
			msg = (wchar_t*)malloc(len * sizeof(wchar_t));

			mbstowcs_s(&msglen, msg, len, buf, _TRUNCATE);

			delete buf;
			return ref new String(msg);
		}
	};

	public ref class AVErrorEventArgs sealed {
	public:
		property int Error {
			int get() {
				return error;
			}
		}
		property String^ Message {
			String^ get() {
				return message;
			}
		}

		String^ GetAVErrorMessage() {
			return AVErrorHelper::GetErrorString(error);
		}

	internal:
		AVErrorEventArgs(int error, String^ message) {
			this->error = error;
			this->message = message;
		}
	private:
		int error;
		String^ message;
	};

	public ref class ErrorContext sealed {
	public:
		/// <summary>
		/// Handles AVError reported by ffmpeg.
		/// This event is reported by another thread.
		/// </summary>
		event AVErrorEventHandler^ AVErrorHandler;

		int ReportError(int code, String^ msg) {
			if (code < 0) {
				DebugMessage(("Error reported: " + msg + ": " + AVErrorHelper::GetErrorString(code) + "\n")->Data());
				concurrency::create_task([=] {
					AVErrorHandler(mss, ref new AVErrorEventArgs(code, msg));
				});
			}

			return code;
		}

		/*void RethrowIfError() {
			int error = Error;
			String^ msg = Message;

			DebugMessage(("Error before thrown: " + msg + " " + AVErrorHelper::GetErrorString(error) + "\n")->Data());
			
			Error = 0;
			Message = nullptr;

			EnsureSuccessOrThrow(error, msg);
		}

		int EnsureSuccessOrThrow(int error, String^ msg) {
			if (error < 0) {
				DebugMessage(("Error thrown: " + msg + " " + AVErrorHelper::GetErrorString(error) + "\n")->Data());
				throw ref new COMException(error, msg + ": " + AVErrorHelper::GetErrorString(error));
			}

			return error;
		}*/

	internal:
		property FFmpegInteropMSS^ mss;
	};
}