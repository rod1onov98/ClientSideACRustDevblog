using Oxide.Core;
using Oxide.Core.Plugins;
using UnityEngine;
using System.Collections.Generic;
using System;

namespace Oxide.Plugins
{
    [Info("AnticheatChecker", "rod1onov98", "2.2.5")]
    [Description("check for using AC")]
    public class AnticheatChecker : RustPlugin
    {
        private readonly string anticheatUrl = "http://127.0.0.1:28015/check-anticheat";
        private Dictionary<ulong, bool> playerAnticheatStatus = new Dictionary<ulong, bool>();
        void OnServerInitialized()
        {
            Puts("AnticheatChecker plugin loaded.");
        }
        void OnPlayerConnected(BasePlayer player)
        {
            CheckAnticheat(player);
        }
        private void CheckAnticheat(BasePlayer player)
        {
            webrequest.Enqueue(anticheatUrl, null, (code, response) =>
            {
                if (code == 200 && !string.IsNullOrEmpty(response))
                {
                    playerAnticheatStatus[player.userID] = true;
                    Puts($"Player {player.displayName} connected with valid AC");
                }
                else
                {
                    playerAnticheatStatus[player.userID] = false;
                    player.Kick("server required AC, for install:");
                    Puts($"Player {player.displayName} was kicked for invalid AC");
                }
            }, this);
        }

        [ChatCommand("acstatus")]
        private void CheckPlayerACStatus(BasePlayer player, string command, string[] args)
        {
            if (args.Length != 1)
            {
                player.ChatMessage("use: /acstatus <player_name>");
                return;
            }
            BasePlayer targetPlayer = null;
            foreach (var p in BasePlayer.activePlayerList)
            {
                if (p.displayName.Equals(args[0], StringComparison.OrdinalIgnoreCase))
                {
                    targetPlayer = p;
                    break;
                }
            }

            if (targetPlayer != null)
            {
                if (playerAnticheatStatus.ContainsKey(targetPlayer.userID))
                {
                    bool isUsingAnticheat = playerAnticheatStatus[targetPlayer.userID];
                    player.ChatMessage($"{targetPlayer.displayName} is using AC: {isUsingAnticheat}");
                }
                else
                {
                    player.ChatMessage($"{targetPlayer.displayName} has not been checked for AC");
                }
            }
            else
            {
                player.ChatMessage("player not found.");
            }
        }
    }
}
