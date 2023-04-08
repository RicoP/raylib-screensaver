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
	bool active;        // NOTE: Use it to activate/deactive particle
} Particle;

Particle mouseTail[MAX_PARTICLES] = { 0 };
int blending = BLEND_ALPHA;
float gravity = 3.0f;
Texture2D smoke;

Vector2 GetTailPosition() 
{
	Vector2 v;

	int x = rand() % GetScreenWidth();
	int y = rand() % GetScreenWidth();

	v.x = (float)x;
	v.y = (float)y;
	return v;
}

void Init()
{
	// Initialize particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		mouseTail[i].position = (Vector2){ 0, 0 };
		mouseTail[i].color = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
		mouseTail[i].alpha = 1.0f;
		mouseTail[i].size = (float)GetRandomValue(1, 30) / 20.0f;
		mouseTail[i].rotation = (float)GetRandomValue(0, 360);
		mouseTail[i].active = false;
	}

	Color from = WHITE;
	Color to = WHITE;
	to.a = 0;
	Image radial = GenImageGradientRadial(128, 128, 0.0f, from, to);
	smoke = LoadTextureFromImage(radial);
	UnloadImage(radial);
}

void Update()
{
	// Update
	   //----------------------------------------------------------------------------------

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
			mouseTail[i].position = GetTailPosition();
			i = MAX_PARTICLES;
		}
	}

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (mouseTail[i].active)
		{
			mouseTail[i].position.y += gravity / 2;
			mouseTail[i].alpha -= 0.005f;

			if (mouseTail[i].alpha <= 0.0f) mouseTail[i].active = false;

			mouseTail[i].rotation += 2.0f;
		}
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		if (blending == BLEND_ALPHA) blending = BLEND_ADDITIVE;
		else blending = BLEND_ALPHA;
	}
	//----------------------------------------------------------------------------------

	// Draw
	//----------------------------------------------------------------------------------
	BeginDrawing();

	ClearBackground(DARKGRAY);

	BeginBlendMode(blending);

	// Draw active particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (mouseTail[i].active) DrawTexturePro(smoke, (Rectangle) { 0.0f, 0.0f, (float)smoke.width, (float)smoke.height },
			(Rectangle) {
			mouseTail[i].position.x, mouseTail[i].position.y, smoke.width* mouseTail[i].size, smoke.height* mouseTail[i].size
		},
			(Vector2) {
			(float)(smoke.width * mouseTail[i].size / 2.0f), (float)(smoke.height * mouseTail[i].size / 2.0f)
		}, mouseTail[i].rotation,
				Fade(mouseTail[i].color, mouseTail[i].alpha));
	}

	EndBlendMode();

	EndDrawing();
}

void Destroy()
{
	UnloadTexture(smoke);
}

