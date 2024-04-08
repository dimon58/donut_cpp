#include <windows.h>
#include <chrono>
#include <thread>

#include "draw.hpp"
#include "fps.hpp"
#include "donut.hpp"

#define CONSOLE_WIDTH 160
#define CONSOLE_HEIGHT 90


long long now_ns() {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}


int main() {

    ConsoleDrawer drawer = ConsoleDrawer(CONSOLE_WIDTH, CONSOLE_HEIGHT);
    drawer.print_terminal_info();

    auto all_frame_stat = FrameStat();
    auto render_frame_stat = FrameStat();
    auto flush_frame_stat = FrameStat();

    std::chrono::duration<int64_t, std::nano> start = std::chrono::steady_clock::now().time_since_epoch(), end{};
    long long start_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(start).count();

    all_frame_stat.last_frame_time = start_ns;
    render_frame_stat.last_frame_time = start_ns;
    flush_frame_stat.last_frame_time = start_ns;

    long long s, e, flush_time;

    auto donut = Donut(CONSOLE_WIDTH / 2, CONSOLE_HEIGHT / 2 + 2, 30, 10);

    while (true) {
        flush_time = 0;

        s = now_ns();
        drawer.reset();
        e = now_ns();
        flush_time += e - s;

        s = now_ns();
        auto casted_time = std::chrono::duration_cast<std::chrono::milliseconds>(start);
        auto casted_time_double = static_cast<double>(casted_time.count()) / 1000.0;
        donut.draw(drawer, 0.75, casted_time_double, casted_time_double / 2);
        e = now_ns();
        render_frame_stat.add_frame(e - s);

        s = now_ns();
//        drawer.draw_z();
        drawer.flush();
        e = now_ns();
        flush_time += e - s;
        flush_frame_stat.add_frame(flush_time);

        end = std::chrono::steady_clock::now().time_since_epoch();
        auto end_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end).count();
        all_frame_stat.update(end_ns);
        start = end;

        if (all_frame_stat.need_display(end_ns)) {
            auto str = std::format("Avg {} | render {} | flush {}", all_frame_stat.get_avg_fps_string(),
                                   render_frame_stat.get_avg_fps_string(), flush_frame_stat.get_avg_fps_string());
            SetConsoleTitle(str.c_str());
            all_frame_stat.last_fps_refresh_time = end_ns;
        }

        //std::chrono::milliseconds timespan(30);
        //std::this_thread::sleep_for(timespan);

    }
    return 0;
}
