//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Hyunjun Jang
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <mutex>
#include <condition_variable>
#include "core/ovdata_structure.h"
namespace ov
{
	class Event
	{
	public:
		explicit Event(bool manual_reset = false);

		// 이벤트 설정
		bool SetEvent();

		bool Reset();

		// timeout in milliseconds
		bool Wait(int timeout = Infinite);
		bool Wait(std::chrono::time_point<std::chrono::system_clock> time_point);

	protected:
		bool _manual_reset;

		std::mutex _mutex;
		std::condition_variable _condition;
		bool _event;
	};
}

