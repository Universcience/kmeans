/*  Quick and dirty implementation of the k-means algorithm (SDL2-based).
 *  Copyright (C) 2018 - Jérôme Kirman
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>

// Screen size
#define WIDTH  800
#define HEIGHT 600

// Border-center and center-datum separation (keep BGAP >= CGAP).
#define BGAP 100
#define CGAP 80

// Number of classes and datums in each class.
#define NCLASS 3
#define NDOTS (10*NCLASS)

// Visual datum size.
#define DSIZE 5

// Data point (with pointer to current centroid)
typedef struct dpoint_s {
	unsigned class;
	unsigned x, y;
	struct dpoint_s* nearest;
} dpoint;

// Class color
int red   (unsigned cl) { return (cl == 0) ? 255 : 0; }
int green (unsigned cl) { return (cl == 1) ? 255 : 0; }
int blue  (unsigned cl) { return (cl == 2) ? 255 : 0; }

// Euclidian distance
unsigned dist (dpoint a, dpoint b)
{
	unsigned dx = a.x - b.x, dy = a.y - b.y;
	return (unsigned) sqrt(dx*dx + dy*dy);
}

// Returns the nearest centroid
dpoint* find_nearest (dpoint datum, dpoint* means)
{
	unsigned nd = -1, ni = -1, d = -1;
	for (unsigned i = 0 ; i < NCLASS ; ++i) {
		d = dist(datum, means[i]);
		if (d < nd) {
			nd = d;
			ni = i;
		}
	}
	return &means[ni];
}

// Recompute centroids for all data
void recompute_nearests (dpoint* data, dpoint* means)
{
	for (unsigned i = 0 ; i < NDOTS ; ++i)
		data[i].nearest = find_nearest(data[i], means);
}

// Reset centroids to random points (if sticky, use existing data points)
void init_means (dpoint* means, dpoint* data, bool sticky)
{
	if (sticky) {
		unsigned rd[NCLASS];
		for (unsigned i = 0 ; i < NCLASS ; ++i) {
			reroll:
			rd[i] = rand()%NDOTS;
			for (unsigned j = 0 ; j < i ; ++j)
				if (rd[i] == rd[j])
					goto reroll; // Centroids must differ
			means[i] = (dpoint) {NCLASS+i, data[rd[i]].x, data[rd[i]].y, NULL};
		}
	} else
		for (unsigned i = 0 ; i < NCLASS ; ++i)
			means[i] = (dpoint) {NCLASS+i, rand()%WIDTH, rand()%HEIGHT, NULL};
}

int main ()
{
	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	SDL_Event e;

	SDL_Window* mw = SDL_CreateWindow("k-Means", 0, 0, WIDTH, HEIGHT, 0);
	SDL_Renderer* r = SDL_CreateRenderer(mw, -1, 0);

	SDL_Rect border = {0, 0, WIDTH-1, HEIGHT-1};

	// Data init
	srand(time(NULL));
	dpoint* centers = calloc(NCLASS, sizeof(dpoint));
	for (unsigned i = 0 ; i < NCLASS ; ++i)
		centers[i] = (dpoint) {
			i,
			rand()%( WIDTH - 2*BGAP) + BGAP,
			rand()%(HEIGHT - 2*BGAP) + BGAP,
			NULL
		};
	//TODO: Reroll centers if distance << 2*CGAP ?

	//NOTE: Biased RNG ; don't care.
	dpoint* data = calloc(NDOTS, sizeof(dpoint));
	for (unsigned i = 0 ; i < NDOTS; ++i)
		data[i] = (dpoint) {
			i%NCLASS,
			centers[i%NCLASS].x + rand()%(2*CGAP) - CGAP,
			centers[i%NCLASS].y + rand()%(2*CGAP) - CGAP,
			NULL
		};

	// Means init
	dpoint* means = calloc(NCLASS, sizeof(dpoint));
	init_means(means, data, true);
	recompute_nearests(data, means);

	// Main loop
	SDL_Rect point = {0, 0, DSIZE, DSIZE};
	bool end = false;
	while (!end) {
		// Handle events
		while (SDL_PollEvent(&e))
			if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_R) {
				// Restart with fresh means (press 'r')
				init_means(means, data, true);
				recompute_nearests(data, means);
			} else if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_N) {
				// Next step : recompute means and nearests (press 'n')
				for (unsigned i = 0 ; i < NCLASS ; ++i) {
					unsigned sx = 0, sy = 0, nd = 0;
					for (unsigned j = 0 ; j < NDOTS ; ++j)
						if (data[j].nearest == &means[i]) {
							sx += data[j].x;
							sy += data[j].y;
							++nd;
						}
					//FIXME: Crashes if nd == 0
					means[i].x = sx / nd;
					means[i].y = sy / nd;
				}
				recompute_nearests(data, means);
			} else if (e.type == SDL_QUIT)
				end = true;

		// Refresh screen
		SDL_RenderPresent(r);
		SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
		SDL_RenderClear(r);
		SDL_RenderDrawRect(r, &border);

		// Draw data ; with colors (press 'h')
		for (unsigned i = 0 ; i < NDOTS ; ++i) {
			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_H])
				SDL_SetRenderDrawColor(r, red(i%NCLASS), green(i%NCLASS), blue(i%NCLASS), 255);
			else
				SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
			point.x = data[i].x;
			point.y = data[i].y;
			SDL_RenderFillRect(r, &point);
			unsigned ncl = data[i].nearest->class % NCLASS;
			SDL_SetRenderDrawColor(r, (255-red(ncl))/4, (255-green(ncl))/4, (255-blue(ncl))/4, 255);
			SDL_RenderDrawLine(r, data[i].x+DSIZE/2, data[i].y+DSIZE/2, data[i].nearest->x+DSIZE/2, data[i].nearest->y+DSIZE/2);
		}
		for (unsigned i = 0 ; i < NCLASS ; ++i) {
			// Draw centers (press 'c')
			SDL_SetRenderDrawColor(r, red(i)/2, green(i)/2, blue(i)/2, 255);
			point.x = centers[i].x;
			point.y = centers[i].y;
			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C])
				SDL_RenderFillRect(r, &point);

			// Draw means
			SDL_SetRenderDrawColor(r, 255-red(i), 255-green(i), 255-blue(i), 255);
			point.x = means[i].x;
			point.y = means[i].y;
			SDL_RenderFillRect(r, &point);
		}
	}

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(mw);
	SDL_Quit();

	free(centers);
	free(means);
	free(data);

	return EXIT_SUCCESS;
}
