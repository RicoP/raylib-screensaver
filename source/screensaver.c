#include "raylib.h"
#include <stdlib.h>

#define MAX_PARTICLES 200

typedef struct {
	Vector2 position;
	Color color;
	float alpha;
	float size;
	float rotation;
	float gravity;
	bool active;
} Particle;

int blending = BLEND_ALPHA;
Particle particles[MAX_PARTICLES] = { 0 };
RenderTexture2D raylibLogo;
int frameCounter = 0;

Vector2 GetRandomPosition() 
{
	Vector2 v;

	int x = rand() % GetScreenWidth();
	int y = rand() % GetScreenHeight();
	y -= 200;

	v.x = (float)x;
	v.y = (float)y;
	return v;
}

void SpawnParticle() 
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (!particles[i].active)
		{
			particles[i].active = true;
			particles[i].alpha = 1.0f;
			particles[i].gravity = ((float)GetRandomValue(30, 70)) / 10.0f;
			particles[i].position = GetRandomPosition();
			particles[i].color = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
			particles[i].size = (float)GetRandomValue(1, 30) / 20.0f;
			particles[i].rotation = (float)GetRandomValue(180 - 45, 180 + 45);
			break;
		}
	}
}

void Init()
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		memset(&particles[i], 0, sizeof(Particle));
	}

	memset(&raylibLogo, 0, sizeof(raylibLogo));
}

void Update()
{
	if(raylibLogo.id == 0)
	{
		raylibLogo = LoadRenderTexture(256, 256);
		BeginTextureMode(raylibLogo);
		{
			ClearBackground(BLACK);
			DrawRectangle(128 - 112, 128 - 112, 224, 224, RAYWHITE);
			DrawText("raylib", 128 - 44, 128 + 48, 50, BLACK);
		}
		EndTextureMode();
	}

	if((frameCounter % 3) == 0) SpawnParticle();
	frameCounter++;

	// Update particle
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (particles[i].active)
		{
			particles[i].position.y += particles[i].gravity / 2;
			particles[i].alpha -= 0.0025f;
			particles[i].active = (particles[i].alpha > 0.0f);
			//mouseTail[i].rotation += 2.0f;
		}
	}

	// Draw
	Texture2D particle = raylibLogo.texture;
	BeginDrawing();

	ClearBackground(DARKGRAY);

	BeginBlendMode(blending);

	// Draw active particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (particles[i].active) 
		{
			DrawTexturePro(
				particle, 
				(Rectangle) { 0.0f, 0.0f, (float)-particle.width, (float)particle.height },
				(Rectangle) { particles[i].position.x, particles[i].position.y, particle.width* particles[i].size, particle.height* particles[i].size },
				(Vector2) { (float)(particle.width * particles[i].size / 2.0f), (float)(particle.height * particles[i].size / 2.0f) }, 
				particles[i].rotation,
				Fade(particles[i].color, particles[i].alpha)
			);
		}
	}

	EndBlendMode();

	EndDrawing();
}

void Destroy()
{
	UnloadRenderTexture(raylibLogo);
}

