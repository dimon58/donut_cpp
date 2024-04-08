#pragma once


struct FrameStat {
    long long frame_number = 0;
    int frame_time_buf_len = 1000;
    long long *frame_time_buf;
    long long fps_refresh_time_ns = 500'000'000;
    long long last_fps_refresh_time = 0;
    long long last_frame_time = 0;

    FrameStat() {
        frame_time_buf = new long long[frame_time_buf_len];
        memset(frame_time_buf, 1000000000.0, frame_time_buf_len);
    }

    void add_frame(long long frame_time_ns) {
        frame_time_buf[frame_number % frame_time_buf_len] = frame_time_ns;
        ++frame_number;
    }

    void update(long long now_ns) {
        add_frame(now_ns - last_frame_time);
        last_frame_time = now_ns;
    }

    [[nodiscard]] std::string get_avg_fps_string() const {
        double avg = 0;
        auto cnt = frame_time_buf_len < frame_number ? frame_time_buf_len : frame_number;
        auto cnt_double = static_cast<double>(cnt);
        for (int j = 0; j < cnt; ++j) {
            avg += static_cast<double>(frame_time_buf[j]) / cnt_double;
        }

        double avg_fps = 1'000'000'000.0 / static_cast<double>(avg);
        return std::format("{:.1f} fps", avg_fps);
    }

    [[nodiscard]] inline bool need_display(long long now_ns) const {
        return now_ns >= last_fps_refresh_time + fps_refresh_time_ns;
    }

    void display() const {
        SetConsoleTitle(get_avg_fps_string().c_str());
    }

    void update_and_display(long long now_ns) {
        update(now_ns);

        if (need_display(now_ns)) {
            display();
            last_fps_refresh_time = now_ns;
        }
    }
};