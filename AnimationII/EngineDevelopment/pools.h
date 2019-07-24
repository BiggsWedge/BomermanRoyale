#pragma once
#include <type_traits>
#include <array>
#include <cassert>
#include <chrono>
#include <random>

namespace end
{
	template<typename T, int16_t N>
	class sorted_pool_t
	{
	public:
		// Todo: Implement the function bodies

		// Returns the number of active elements
		size_t size()const { return active_count; }

		// Returns the maximum supported number of elements 
		size_t capacity()const { return N; }

		// Returns the value at the specified index
		T& operator[](int16_t index) { return pool[index]; }

		// Returns the value at the specified index
		const T& operator[](int16_t index)const { return pool[index]; }

		// Returns the index of the first inactive element 
		//   and updates the active count
		// Returns -1 if no inactive elements remain
		int16_t alloc()
		{
			if (active_count + 1 >= N)
				return -1;
			else
				return active_count++;
		}

		// Moves the element at 'index' to the inactive
		// region and updates the active count
		void free(int16_t index)
		{
			pool[index] = pool[--active_count];
		}

	private:
		T pool[N];
		int16_t active_count = 0;
	};

	template<typename T, int16_t N>
	class pool_t
	{
	public:
		// Todo: Implement the function bodies

		// Removes the first element from the free list and returns its index
		// Returns -1 if no free elements remain
		int16_t alloc()
		{
			int16_t index = free_start;
			if (free_start >= N)
				return -1;
			free_start = pool[index].next;
			return index;
		}

		// Adds 'index' to the free list
		void free(int16_t index)
		{
			pool[index].next = free_start;
			free_start = index;
		}


		// Initializes the free list
		pool_t()
		{
			for (int i = 0; i < N;i++)
			{
				pool[i].next = i + 1;
			}
		}

		// Returns the value at the specified index
		T& operator[](int16_t index)
		{
			return pool[index].value;
		}

		// Returns the value at the specified index
		const T& operator[](int16_t index)const
		{
			return pool[index].value;
		}

	private:

		union element_t
		{
			T value;
			int16_t next;

			element_t()
			{

			}
		};

		element_t pool[N];

		int16_t free_start = 0;
	};

	struct particle
	{
		float3 pos;
		float3 prevPos;
		float3 origin;
		float4 color;
		float3 dir;
		float speed;

		particle& operator=(particle _other)
		{
			if (this != &_other)
			{
				this->color = _other.color;
				this->pos = _other.pos;
				this->prevPos = _other.prevPos;
				this->dir = _other.dir;
				this->origin = _other.origin;
				this->speed = _other.speed;
			}
			return *this;
		}

		particle()
		{
			pos = { 0.0f, 0.0f, 0.0f };
			dir = { 0.0f, 0.0f, 0.0f };
			prevPos = { 0.0f, 0.0f, 0.0f };
			color = { 0.0f, 0.0f, 0.0f, 0.0f };
			origin = { 0.0f, 0.0f, 0.0f };
			speed = 0.0f;
		}
		particle(float3 _dir, float4 _color = { 0.0f, 0.0f, 0.0f, 0.0f }, float3 _origin = { 0.0f, 0.0f, 0.0f })
		{
			prevPos = { 0.0f, 0.0f, 0.0f };
			color = { 0.0f, 0.0f, 0.0f, 0.0f };
			pos = { 0.0f, 0.0f,0.0f };
			origin = { 0.0f ,0.0f, 0.0f };
			speed = 0.0f;

			dir = _dir;
			if (!(_origin == 0.0f))
			{
				pos = _origin;
				origin = _origin;
				prevPos = _origin;
			}
			if (!(_color == 0.0f))
				color = _color;
		}
	};

	struct emitter
	{
		float3 spawnPoint;
		float4 spawnColor;

		sorted_pool_t<int16_t, 128> indices;

		pool_t<particle, 512>* _usedPool;

		int spawnParticle()
		{
			int newIndex = indices.alloc();
			if (newIndex == -1)
				return -1;
			int partIndex = _usedPool->alloc();
			if (partIndex == -1)
				return -1;
			indices[newIndex] = partIndex;

			particle temp;
			temp.color = spawnColor;


			srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());

			float3 currSpawn = { ((rand() % 2000) - 1000.0f) / 1000.0f, 0.0f,((rand() % 2000) - 1000.0f) / 1000.0f };
			temp.pos = temp.origin = temp.prevPos = (spawnPoint + currSpawn);
			temp.speed = 0.0f;

			(*_usedPool)[indices[newIndex]] = temp;
			return newIndex;
		}

		void updateParticles(float timePassed)
		{
			float3 _dest = { spawnPoint.x, spawnPoint.y + 5.0f, spawnPoint.z };
			for (int i = 0; i < indices.size(); ++i)
			{
				int partIndex = indices[i];
				particle* part = &(*_usedPool)[partIndex];
				if (part->speed >= 5.0f)
				{
					freeParticle(i);
					i--;
					continue;
				}
				part->prevPos = part->pos;
				part->pos = positionLerp(part->origin, _dest, findRatio(0.0f, 5.0f, part->speed));
				part->speed += timePassed;
			}
			int result = spawnParticle();
			if (result == -1)
				return;

		}

		void freeParticle(int index)
		{
			_usedPool->free(indices[index]);
			indices.free(index);
		}

		particle* operator[](int16_t index)
		{
			return &_usedPool->operator[](indices[index]);
		}

	};
}