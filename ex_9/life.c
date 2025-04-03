	#define _XOPEN_SOURCE 500
	#include <ncurses.h>
	#include <locale.h>
	#include <unistd.h>
	#include <stdbool.h>
	#include "grid.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <pthread.h>
	#include <signal.h>
	#include <math.h>

	const int width = 30;
	const int height = 30;

	typedef struct {
		int start;
		int end;
		char** foreground;
		char** background;
	} thread_data;

	void handler (int signo) {
		return;
	}

	void* thread_func(void* arg)
	{
		thread_data *data = (thread_data *)arg;
		int start = data->start;
		int end = data->end;
		while (true)
		{
			pause();
			for (int i=start; i<end; i++)
			{
				int row = i / width;
				int col = i % width;
				(*data->background)[i] = is_alive(row, col, *data->foreground);
			}
		}
	}

	int main(int argc, char *argv[])
	{
		int n = atoi(argv[1]);
		if (n > width * height){
			return 0;
		}

		pthread_t thread_arr[n];
		thread_data data_arr[n];

		int cells_per_thread = (int)ceil(width * height / n);

		struct sigaction sa;
		sa.sa_handler = handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sigaction(SIGUSR1, &sa, NULL);

		srand(time(NULL));
		setlocale(LC_CTYPE, "");
		initscr(); // Start curses mode

		char *foreground = create_grid();
		char *background = create_grid();
		char *tmp;
		
		for (int i=0; i<n; i++){
			data_arr[i].start = i * cells_per_thread;
			if ((i+1)* cells_per_thread < width * height)
				data_arr[i].end = (i+1) * cells_per_thread;
			else
				data_arr[i].end = width * height;

			data_arr[i].foreground = &foreground;
			data_arr[i].background = &background;
			pthread_create(&thread_arr[i], NULL, thread_func, &data_arr[i]);
		}

		init_grid(foreground);

		while (true)
		{
			draw_grid(foreground);

			for (int i=0; i<n; i++){
				pthread_kill(thread_arr[i], SIGUSR1);
			}
			usleep(500 * 1000);

			// Step simulation
			tmp = foreground;
			foreground = background;
			background = tmp;
		}

		endwin(); // End curses mode
		destroy_grid(foreground);
		destroy_grid(background);

		return 0;
	}
