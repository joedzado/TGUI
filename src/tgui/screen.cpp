#include "screen.hpp"

namespace tgui {
	namespace screen {
		void set_cursor_position ( coord_type column, coord_type row ) {
			set_cursor_position( { column, row } );
		}

		void move_cursor_position ( coord_type column, coord_type row ) {
			move_cursor_position( { column, row } );
		}

#if defined(TGUI_LINUX)

		// Details

		// Actual Implementation
		position get_size() {
			winsize w;
			position size;

			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

			size.column = w.ws_col;
			size.row = w.ws_row;

			return size;
		}

		void set_cursor_position(const position& pos) {
			std::cout << details::csi << pos.row << ';' << pos.column << 'H';
		}

		void move_cursor_position(const position& pos) {
			std::cout << details::csi;

			if(pos.column < 0) {
				std::cout << pos.column << 'D';
			} else if(pos.column > 0) {
				std::cout << pos.column << 'C';
			}

			std::cout << details::csi;

			if(pos.row < 0) {
				std::cout << pos.row << 'A';
			} else if(pos.row > 0) {
				std::cout << pos.row << 'B';
			}
		}

		void set_color(const color& foreground, const color& background) {
			set_background_color(background);
			set_foreground_color(foreground);
		}

		void set_foreground_color(const color& foreground) {
			int intensity = (static_cast<int>(foreground) & 8)? 1 : 22;
			int color = (static_cast<int>(foreground) & 7) + 30;

			std::cout << details::csi << intensity << ';' << color << 'm';
		}

		void set_background_color(const color& background) {
			int intensity = (static_cast<int>(background) & 8)? 1 : 22;
			int color = (static_cast<int>(background) & 7) + 40;

			std::cout << details::csi << intensity << ';' << color << 'm';
		}

#elif defined(TGUI_WINDOWS)

		// Details
		CONSOLE_SCREEN_BUFFER_INFO details::get_console_screen_buffer_info () {
			CONSOLE_SCREEN_BUFFER_INFO csbi;

			GetConsoleScreenBufferInfo( hStdout, &csbi );

			return csbi;
		}

		position details::get_cursor_position () {
			CONSOLE_SCREEN_BUFFER_INFO csbi = details::get_console_screen_buffer_info();
			position size;

			size.column = csbi.dwCursorPosition.X;
			size.row = csbi.dwCursorPosition.Y;

			return size;
		}

		// Actual Implementation
		position get_size () {
			CONSOLE_SCREEN_BUFFER_INFO csbi = details::get_console_screen_buffer_info();
			position size;

			size.column = csbi.dwMaximumWindowSize.X;
			size.row = csbi.dwMaximumWindowSize.Y;

			return size;
		}

		void set_cursor_position ( const position& pos ) {
			COORD cursor( { pos.row, pos.column } );

			SetConsoleCursorPosition( details::hStdout, cursor );
		}

		void move_cursor_position ( const position& pos ) {
			position cur_pos = details::get_cursor_position();

			set_cursor_position( cur_pos.row + pos.row, cur_pos.column + pos.column );
		}

		void set_color ( const color& foreground, const color& background ) {
			SetConsoleTextAttribute( details::hStdout, details::foreground_colors.at( foreground ) | details::background_colors.at( background ) );
		}

		void set_foreground_color ( const color& foreground ) {
			SetConsoleTextAttribute( details::hStdout, (details::get_console_screen_buffer_info().wAttributes & 0xf0) | details::foreground_colors.at( foreground ) );
		}

		void set_background_color ( const color& background ) {
			SetConsoleTextAttribute( details::hStdout, (details::get_console_screen_buffer_info().wAttributes & 0x0f) | details::background_colors.at( background ) );
		}

#endif

		void ring_bell () {
			std::cout << '\a';
		}

		void flush_screen () {
			std::cout.flush();
		}
	}
}
