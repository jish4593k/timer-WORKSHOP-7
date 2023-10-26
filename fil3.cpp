#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void countdownTimer(int minutes, int seconds = 0) {
    int totalSeconds = (minutes * 60) + seconds;
    while (totalSeconds > 0) {
        int mins = totalSeconds / 60;
        int secs = totalSeconds % 60;
        std::cout << "\rTime Remaining: " << (mins < 10 ? "0" : "") << mins << ":" << (secs < 10 ? "0" : "") << secs << " ";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        totalSeconds--;
    }
    std::cout << "\nTime is up!" << std::endl;
}

void playAudio(const char* filePath) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        const char* cmd = "aplay";  // Use 'aplay' for Linux, adjust for other platforms
        char* const args[] = {(char*)cmd, (char*)filePath, nullptr};
        execvp(cmd, args);
        exit(0);  // Exit the child process
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            std::cout << "Audio playback finished." << std::endl;
        } else {
            std::cerr << "Audio playback error. Exit code: " << WEXITSTATUS(status) << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <minutes> [-s <seconds>] [-a <audio_path>]" << std::endl;
        return 1;
    }

    int minutes = std::stoi(argv[1]);
    int seconds = 0;
    int audioIndex = -1;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            if (i < argc - 1) {
                seconds = std::stoi(argv[i + 1]);
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            if (i < argc - 1) {
                audioIndex = i + 1;
            }
        }
    }

    const char* audioPath = (audioIndex != -1) ? argv[audioIndex] : nullptr;

    std::cout << "Starting a " << minutes << " minute and " << seconds << " second countdown" << std::endl;
    countdownTimer(minutes, seconds);

    if (audioPath) {
        playAudio(audioPath);
    }

    std::cout << "Timer finished!" << std::endl;
    return 0;
}
