#pragma once

#include "Types.hpp"

namespace paa
{
    class MusicPlayer
    {
    private:
        paa::Music _music;

    public:
        MusicPlayer() = default;
        ~MusicPlayer() = default;

        bool play(const std::string& path, bool vloop=true)
        {
            if (!_music.openFromFile(path))
                return false;
            _music.play();
            loop(vloop);
            return true;
        }

        void pause()
        {
            _music.pause();
        }

        void stop()
        {
            _music.stop();
        }

        void loop(bool vloop)
        {
            _music.setLoop(vloop);
        }

        void setLoopPoints(double start_sec, double length_sec)
        {
            sf::Music::TimeSpan t(sf::seconds(start_sec), sf::seconds(length_sec));
            _music.setLoopPoints(t);
        }
    };

    class GMusicPlayer
    {
    private:
        static inline MusicPlayer *_musicPlayer = nullptr;
        static inline std::string playedMusic;

    public:
        GMusicPlayer() = default;
        ~GMusicPlayer() = default;

        static MusicPlayer& get()
        {
            if (_musicPlayer == nullptr) {
                _musicPlayer = new MusicPlayer;
            }
            return *_musicPlayer;
        }

        static void release()
        {
            if (_musicPlayer != nullptr) {
                delete _musicPlayer;
                _musicPlayer = nullptr;
            }
        }

        static void playIfNotPlayed(const std::string& path, bool loop = true) {
            setLoop(loop);
            if (playedMusic == path)
                return;
            play(path, loop);
        }

        static void play(const std::string& path, bool loop=true)
        {
            spdlog::info("GMusicPlayer::play: {}", path);
            playedMusic = path;
            get().play(path, loop);
        }

        static std::string getPlayedMusic() {
            return playedMusic;
        }

        static void pause()
        {
            get().pause();
        }

        static void stop()
        {
            get().stop();
        }

        static void setLoop(bool loop)
        {
            get().loop(loop);
        }

        static void setLoopPoints(double start_sec, double length_sec)
        {
            get().setLoopPoints(start_sec, length_sec);
        }
    };
}
