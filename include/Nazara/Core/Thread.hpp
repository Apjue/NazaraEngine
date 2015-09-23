// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_THREAD_HPP
#define NAZARA_THREAD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Functor.hpp>
#include <iosfwd>

class NzThreadImpl;

class NAZARA_CORE_API NzThread
{
	public:
		class Id;

		NzThread();
		template<typename F> NzThread(F function);
		template<typename F, typename... Args> NzThread(F function, Args&&... args);
		template<typename C> NzThread(void (C::*function)(), C* object);
		NzThread(const NzThread&) = delete;
		NzThread(NzThread&& other);
		~NzThread();

		void Detach();
		Id GetId() const;
		bool IsJoinable() const;
		void Join();

		NzThread& operator=(const NzThread&) = delete;
		NzThread& operator=(NzThread&& thread);

		static unsigned int HardwareConcurrency();
		static void Sleep(nzUInt32 milliseconds);

	private:
		void CreateImpl(NzFunctor* functor);

		NzThreadImpl* m_impl;
};

class NAZARA_CORE_API NzThread::Id
{
	friend NzThread;

	public:
		NAZARA_CORE_API friend bool operator==(const Id& lhs, const Id& rhs);
		NAZARA_CORE_API friend bool operator!=(const Id& lhs, const Id& rhs);
		NAZARA_CORE_API friend bool operator<(const Id& lhs, const Id& rhs);
		NAZARA_CORE_API friend bool operator<=(const Id& lhs, const Id& rhs);
		NAZARA_CORE_API friend bool operator>(const Id& lhs, const Id& rhs);
		NAZARA_CORE_API friend bool operator>=(const Id& lhs, const Id& rhs);

		NAZARA_CORE_API friend std::ostream& operator<<(std::ostream& o, const Id& id);

	private:
		Id(NzThreadImpl* thread);

		NzThreadImpl* m_id = nullptr;
};

#include <Nazara/Core/Thread.inl>

#endif // NAZARA_THREAD_HPP
