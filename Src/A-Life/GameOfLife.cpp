#include "GameOfLife.h"
#include "Texture.h"
#include "Random.h"

bool GameOfLife::Initialize()
{
	bufferA.resize(size.x * size.y);
	bufferB.resize(size.x * size.y);

	return true;
}

void GameOfLife::Step()
{
    std::vector<uint8_t>& readBuffer = (frame % 2) ? bufferA : bufferB;
    std::vector<uint8_t>& writeBuffer = (frame % 2) ? bufferB : bufferA;
    timer = 50;
    if (timer >= 50 && work)
    {
        timer = 0;

        // update buffer
        // clear write buffer
        std::fill(writeBuffer.begin(), writeBuffer.end(), 0);

        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.x; x++)
            {
                int weight = 0;

                // read surrounding 8 cells, accumulate weight
                // XXX
                // X0X
                // XXX
                weight += Read<uint8_t>(readBuffer, x - 1, y + 1);
                weight += Read<uint8_t>(readBuffer, x - 1, y);
                weight += Read<uint8_t>(readBuffer, x - 1, y - 1);
                weight += Read<uint8_t>(readBuffer, x, y + 1);
                weight += Read<uint8_t>(readBuffer, x, y - 1);
                weight += Read<uint8_t>(readBuffer, x + 1, y + 1);
                weight += Read<uint8_t>(readBuffer, x + 1, y);
                weight += Read<uint8_t>(readBuffer, x + 1, y - 1);

                // game of life rules
               // if cell is alive, update
                if (Read<uint8_t>(readBuffer, x, y))
                {
                    if (weight < 2 || weight > 3) Write<uint8_t>(writeBuffer, x, y, 0);
                    else Write<uint8_t>(writeBuffer, x, y, 1);
                }
                else
                {
                    // if cell is dead, update
                    if (weight == 3) Write<uint8_t>(writeBuffer, x, y, 1);
                }
            }
        }

        // convert buffer data format to color buffer
        std::transform((writeBuffer).begin(), (writeBuffer).end(), color_buffer.begin(), [](uint8_t v)
            {
                { return  v ? 0xffffffff : 0; }
            });

        // copy color buffer to texture
        texture->Copy(color_buffer);
        work = false;
    }
    else {
        if (mouse_button[1])
        {
            Write<uint8_t>(writeBuffer, mouse_position.x, mouse_position.y, 1);
        }

        // convert buffer data format to color buffer
        std::transform((writeBuffer).begin(), (writeBuffer).end(), color_buffer.begin(), [](uint8_t v)
            {
                { return  v ? 0xffffffff : 0; }
            });

        // copy color buffer to texture
        texture->Copy(color_buffer);
    }
}

void GameOfLife::KeyDown(SDL_Keycode keycode)
{
    if (keycode == SDLK_r)
    {
        // write random 'alive' cells to buffer
        std::vector<uint8_t>& writeBuffer = (frame % 2) ? bufferB : bufferA;
        for (int i = 0; i < writeBuffer.size(); i++) writeBuffer[i] = (random(0, 10) == 0) ? 1 : 0;
    }
    if (keycode == SDLK_SPACE) {
        work = true;
        frame++;
    }
    if (keycode == SDLK_u) {
        std::fill(bufferA.begin(), bufferA.end(), 0);
        std::fill(bufferB.begin(), bufferB.end(), 0);
    }
}