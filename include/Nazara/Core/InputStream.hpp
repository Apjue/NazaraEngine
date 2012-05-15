// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTSTREAM_HPP
#define NAZARA_INPUTSTREAM_HPP

#include <Nazara/Prerequesites.hpp>

class NzInputStream
{
	public:
		virtual ~NzInputStream();

		virtual bool EndOfFile() const = 0;
		virtual std::size_t Read(void* buffer, std::size_t size) = 0;
};

#endif // NAZARA_INPUTSTREAM_HPP
