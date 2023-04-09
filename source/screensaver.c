#include "raylib.h"
#include <stdlib.h>
#include "raylibscreensaver.h"

enum {
	MAX_PARTICLES = 200,
	BLENDING = BLEND_ALPHA
};

typedef struct {
	Vector2 position;
	Vector2 velocity;
	Color color;
	float alpha;
	float size;
	float rotation;
	bool active;
} Particle;

RenderTexture2D raylibLogo = { 0 };
Particle particles[MAX_PARTICLES] = { 0 };
bool flight = false;

Vector2 GetRandomPosition() 
{
	Vector2 v = { 0,0 };
	if(flight)
	{
		v.x = GetScreenWidth() / 2;
		v.y = GetScreenHeight() / 2;
	}
	else
	{
		v.x = GetRandomDoubleScaler() * GetScreenWidth();
		v.y = GetRandomDoubleScaler() * GetScreenHeight() - GetScreenHeight() - 400;
	}
	return v;
}

Vector2 GetRandomVelocity()
{
	Vector2 v = { 0,0 };
	
	if (flight) 
	{
		v.x = (float)GetRandomDoubleRangeRandomSign(10, 30);
		v.y = (float)GetRandomDoubleRangeRandomSign(10, 30);
	}
	else
	{
		v.y = (float)GetRandomDoubleRange(3, 7);
	}

	return v;
}

// Define a comparison function for qsort to use
int compare_particle_size(const void* a, const void* b) {
	const Particle* particle_a = (const Particle*)a;
	const Particle* particle_b = (const Particle*)b;

	if (particle_a->size < particle_b->size) {
		return -1;
	}
	else if (particle_a->size > particle_b->size) {
		return 1;
	}
	else {
		return 0;
	}
}

void SpawnParticle(bool sort) 
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (!particles[i].active)
		{
			particles[i].active = true;
			particles[i].alpha = 1.0f;
			particles[i].velocity = GetRandomVelocity();
			particles[i].position = GetRandomPosition();
			particles[i].rotation = GetRandomDoubleRange(180 - 45, 180 + 45);
			particles[i].color = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
			particles[i].size = (float)GetRandomDoubleRange(1 / 20.0f, 30 / 20.0f);

			particles[i].velocity.x = 0;
			particles[i].velocity.y = particles[i].size * 15;

			if(sort)
			{
				// Call qsort to sort the particles array by size
				qsort(particles, MAX_PARTICLES, sizeof(Particle), compare_particle_size);
			}
			return;
		}
	}
}

void UpdateParticle()
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (particles[i].active)
		{
			particles[i].position.x += particles[i].velocity.x;
			particles[i].position.y += particles[i].velocity.y;
			//if (particles[i].position.y > 0) particles[i].alpha -= 0.0025f;
			particles[i].active = (particles[i].alpha > 0.0f);
			particles[i].active &= particles[i].position.y < GetScreenHeight() + 300;
			//particles[i].rotation += 2.0f;
		}
	}
}

void Init()
{
	memset(particles, 0, sizeof(Particle) * MAX_PARTICLES);
	memset(&raylibLogo, 0, sizeof(raylibLogo));

	raylibLogo = LoadRenderTexture(256, 256);
	BeginTextureMode(raylibLogo);
	{
		ClearBackground(BLACK);
		DrawRectangle(128 - 112, 128 - 112, 224, 224, RAYWHITE);
		DrawText("raylib", 128 - 44, 128 + 48, 50, BLACK);
	}
	EndTextureMode();

	for (int i = 2000; i--;)
	{
		SpawnParticle(false);
		UpdateParticle();
	}

	//SetWindowPosition(2000, 0);
}

void Update()
{
	static float globalAlpha = 1;
	
	SpawnParticle(true);
	UpdateParticle();
	
	// Draw
	Texture2D particle = raylibLogo.texture;
	BeginDrawing();
	{
		ClearBackground(DARKGRAY);
		BeginBlendMode(BLENDING);
		{
			// Draw active particles
			for (int i = 0; i < MAX_PARTICLES; i++)
			{
				if (!particles[i].active) continue;
				
				DrawTexturePro(
					particle, 
					(Rectangle) { 0.0f, 0.0f, (float)-particle.width, (float)particle.height },
					(Rectangle) { particles[i].position.x, particles[i].position.y, particle.width* particles[i].size, particle.height* particles[i].size },
					(Vector2) { (float)(particle.width * particles[i].size / 2.0f), (float)(particle.height * particles[i].size / 2.0f) }, 
					particles[i].rotation,
					Fade(particles[i].color, particles[i].alpha * globalAlpha)
				);
			}
		}
		EndBlendMode();
	}

	EndDrawing();

	globalAlpha += .005f;
	if(globalAlpha > 1) globalAlpha = 1;
}

void Destroy()
{
	UnloadRenderTexture(raylibLogo);
}
