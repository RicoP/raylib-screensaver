#include "raylib.h"
#include <stdlib.h>


#define MAX_PARTICLES 200

// Particle structure with basic data
typedef struct {
	Vector2 position;
	Color color;
	float alpha;
	float size;
	float rotation;
	float gravity;
	bool active;        // NOTE: Use it to activate/deactive particle
} Particle;

int blending = BLEND_ALPHA;
Particle mouseTail[MAX_PARTICLES] = { 0 };
RenderTexture2D raylibLogo;

int frameCounter = 0;

Vector2 GetTailPosition() 
{
	Vector2 v;

	int x = rand() % GetScreenWidth();
	int y = rand() % GetScreenHeight();
	y -= 100;

	v.x = (float)x;
	v.y = (float)y;
	return v;
}

void SpawnParticle() 
{
	// Activate one particle every frame and Update active particles
	// NOTE: Particles initial position should be mouse position when activated
	// NOTE: Particles fall down with gravity and rotation... and disappear after 2 seconds (alpha = 0)
	// NOTE: When a particle disappears, active = false and it can be reused.
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (!mouseTail[i].active)
		{
			mouseTail[i].active = true;
			mouseTail[i].alpha = 1.0f;
			mouseTail[i].gravity = ((float)GetRandomValue(30, 70)) / 10.0f;
			mouseTail[i].position = GetTailPosition();
			mouseTail[i].color = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
			mouseTail[i].size = (float)GetRandomValue(1, 30) / 20.0f;
			mouseTail[i].rotation = (float)GetRandomValue(0, 360);
			break;
		}
	}
}

void Init()
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		memset(&mouseTail[i], 0, sizeof(Particle));
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
		if (mouseTail[i].active)
		{
			mouseTail[i].position.y += mouseTail[i].gravity / 2;
			mouseTail[i].alpha -= 0.0025f;
			mouseTail[i].active = (mouseTail[i].alpha > 0.0f);
			//mouseTail[i].rotation += 2.0f;
		}
	}

	// Draw
	Texture2D smoke = raylibLogo.texture;
	BeginDrawing();

	ClearBackground(DARKGRAY);

	BeginBlendMode(blending);

	// Draw active particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (mouseTail[i].active) 
		{
			DrawTexturePro(
				smoke, 
				(Rectangle) { 0.0f, 0.0f, (float)-smoke.width, (float)smoke.height },
				(Rectangle) { mouseTail[i].position.x, mouseTail[i].position.y, smoke.width* mouseTail[i].size, smoke.height* mouseTail[i].size },
				(Vector2) { (float)(smoke.width * mouseTail[i].size / 2.0f), (float)(smoke.height * mouseTail[i].size / 2.0f) }, 
				mouseTail[i].rotation,
				Fade(mouseTail[i].color, mouseTail[i].alpha)
			);
		}
	}

	EndBlendMode();

	EndDrawing();
}

void Destroy()
{
	//UnloadTexture(smoke);
	UnloadRenderTexture(raylibLogo);
}

