#include "glm/glm.hpp"

namespace GameConstants
{
   static constexpr float shakeTime       = 0.3f;
   static constexpr float initPadSpeed    = 500.0f;
   static constexpr float initBallRadius  = 15.0f;
   static constexpr glm::vec2 initPadSize = {100.0f, 20.0f};
   static constexpr glm::vec2 initPadPos  = {static_cast<float>(800) / 2.0f - initPadSize.x / 2.0f, static_cast<float>(600) - initPadSize.y};
   static constexpr glm::vec2 initBallPos = {initPadPos.x + initPadSize.x / 2.0f - initBallRadius, initPadPos.y - initBallRadius * 2.0f};
   static constexpr glm::vec2 ballInitVel = glm::vec2{0.0f, -350.0f};

   static constexpr glm::vec2 initPowerUpVel = {0.0f, 150.0f};

   static constexpr float effectSpeedUpTime        = 5.0f;
   static constexpr float speedUpMultiplier        = 1.3f;

   static constexpr float effectIncreaseTime       = 5.0f;
   static constexpr float effectIncreaseMultiplier = 1.5f;

   static constexpr float effectConfuseTime        = 5.0f;
   static constexpr float effectChaosTime          = 5.0f;
}