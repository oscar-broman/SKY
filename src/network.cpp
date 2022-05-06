#include "network.hpp"

bool PlayerSync::received(IPlayer &peer, NetworkBitStream &bs)
{
    int playerId = peer.getID();

    if (Player::GetSyncFrozenState(playerId, SyncTypes::E_PLAYER_SYNC) || Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
    {
        bs.reset();
        Player::lastSyncData[playerId].write(bs);
    }
    else
    {
        Player::lastSyncData[playerId].read(bs);

        if (Player::lastSyncData[playerId].Weapon > PlayerWeapon::PlayerWeapon_Parachute || (Player::lastSyncData[playerId].Weapon > PlayerWeapon::PlayerWeapon_Moltov && Player::lastSyncData[playerId].Weapon < PlayerWeapon::PlayerWeapon_Colt45))
        {
            Player::lastSyncData[playerId].Weapon = PlayerWeapon::PlayerWeapon_Fist;
        }

        // Because of detonator crasher - Sends AIM_KEY in this packet and cam mode IDs 7, 8, 34, 45, 46, 51 and 65 in ID_AIM_SYNC
        if (Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_Bomb)
        {
            Player::lastSyncData[playerId].Keys &= ~128;
        }

        if (disableSyncBugs)
        {
            // Prevent "ghost shooting" bugs
            if ((Player::lastSyncData[playerId].Weapon >= PlayerWeapon::PlayerWeapon_Colt45 && Player::lastSyncData[playerId].Weapon <= PlayerWeapon::PlayerWeapon_Sniper) || Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_Minigun)
            {
                switch (Player::lastSyncData[playerId].AnimationID)
                {
                    // PED_RUN_*
                case 1222:
                case 1223:
                case 1224:
                case 1225:
                case 1226:
                case 1227:
                case 1228:
                case 1229:
                case 1230:
                case 1231:
                case 1232:
                case 1233:
                case 1234:
                case 1235:
                case 1236:
                    // PED_SWAT_RUN
                case 1249:
                    // PED_WOMAN_(RUN/WALK)_*
                case 1275:
                case 1276:
                case 1277:
                case 1278:
                case 1279:
                case 1280:
                case 1281:
                case 1282:
                case 1283:
                case 1284:
                case 1285:
                case 1286:
                case 1287:
                    // FAT_FATRUN_ARMED
                case 459:
                    // MUSCULAR_MUSCLERUN*
                case 908:
                case 909:
                    // PED_WEAPON_CROUCH
                case 1274:
                    // PED_WALK_PLAYER
                case 1266:
                    // PED_SHOT_PARTIAL(_B)
                case 1241:
                case 1242:
                    // Baseball bat
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                    // Knife
                case 745:
                case 746:
                case 747:
                case 748:
                case 749:
                case 750:
                case 751:
                case 752:
                case 753:
                case 754:
                case 755:
                case 756:
                case 757:
                case 758:
                case 759:
                case 760:
                    // Sword
                case 1545:
                case 1546:
                case 1547:
                case 1548:
                case 1549:
                case 1550:
                case 1551:
                case 1552:
                case 1553:
                case 1554:
                    // Fight
                case 471:
                case 472:
                case 473:
                case 474:
                case 477:
                case 478:
                case 479:
                case 480:
                case 481:
                case 482:
                case 483:
                case 484:
                case 485:
                case 486:
                case 487:
                case 488:
                case 489:
                case 490:
                case 491:
                case 492:
                case 493:
                case 494:
                case 495:
                case 496:
                case 497:
                case 498:
                case 499:
                case 500:
                case 501:
                case 502:
                case 503:
                case 504:
                case 505:
                case 506:
                case 507:
                case 1135:
                case 1136:
                case 1137:
                case 1138:
                case 1139:
                case 1140:
                case 1141:
                case 1142:
                case 1143:
                case 1144:
                case 1145:
                case 1146:
                case 1147:
                case 1148:
                case 1149:
                case 1150:
                case 1151:
                    // Only remove action key if holding aim
                    if (Player::lastSyncData[playerId].Keys & 128)
                    {
                        Player::lastSyncData[playerId].Keys &= ~1;
                    }

                    // Remove fire key
                    Player::lastSyncData[playerId].Keys &= ~4;

                    // Remove aim key
                    Player::lastSyncData[playerId].Keys &= ~128;

                    break;
                }
            }
            else if (Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_SprayCan || Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_FireExtinguisher || Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_FlameThrower)
            {
                if (Player::lastSyncData[playerId].AnimationID < 1160 || Player::lastSyncData[playerId].AnimationID > 1167)
                {
                    // Only remove action key if holding aim
                    if (Player::lastSyncData[playerId].Keys & 128)
                    {
                        Player::lastSyncData[playerId].Keys &= ~1;
                    }

                    // Remove fire key
                    Player::lastSyncData[playerId].Keys &= ~4;

                    // Remove aim key
                    Player::lastSyncData[playerId].Keys &= ~128;
                }
            }
            else if (Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_Grenade)
            {
                if (Player::lastSyncData[playerId].AnimationID < 644 || Player::lastSyncData[playerId].AnimationID > 646)
                {
                    Player::lastSyncData[playerId].Keys &= ~1;
                }
            }
        }

        if (Player::blockKeySync[playerId])
        {
            Player::lastSyncData[playerId].Keys = 0;
        }

        if (Player::fakeHealth[playerId] != 255)
        {
            Player::lastSyncData[playerId].HealthArmour[0] = Player::fakeHealth[playerId];
        }

        if (Player::fakeArmour[playerId] != 255)
        {
            Player::lastSyncData[playerId].HealthArmour[1] = Player::fakeArmour[playerId];
        }

        if (Player::fakeQuat[playerId] != nullptr)
        {
            GTAQuat q(
                Player::fakeQuat[playerId]->w,
                Player::fakeQuat[playerId]->x,
                Player::fakeQuat[playerId]->y,
                Player::fakeQuat[playerId]->z);

            Player::lastSyncData[playerId].Rotation = q;
        }

        if (Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_Night_Vis_Goggles || Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_Thermal_Goggles)
        {
            Player::lastSyncData[playerId].Keys &= ~4;
        }
        else if (Player::lastSyncData[playerId].Weapon == PlayerWeapon::PlayerWeapon_Knife && knifeSync == false)
        {
            Player::lastSyncData[playerId].Keys &= ~128;
        }

        Player::lastWeapon[playerId] = Player::lastSyncData[playerId].Weapon;

        bs.reset();
        Player::lastSyncData[playerId].write(bs);
    }

    Player::lastUpdateTick[playerId] = SkyComponent::getCore()->getTickCount();
    Player::lastSyncPacket[playerId] = SyncTypes::E_PLAYER_SYNC;

    return true;
}

bool AimSync::received(IPlayer &peer, NetworkBitStream &bs)
{
    int playerId = peer.getID();

    if (Player::GetSyncFrozenState(playerId, SyncTypes::E_PLAYER_SYNC) || Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
    {
        bs.reset();
        Player::lastPlayerAimSyncData[playerId].write(bs);
    }
    else
    {
        Player::lastPlayerAimSyncData[playerId].read(bs);

        // Fix first-person up/down aim sync
        if (Player::lastWeapon[playerId] == PlayerWeapon_Sniper || Player::lastWeapon[playerId] == PlayerWeapon_RocketLauncher || Player::lastWeapon[playerId] == PlayerWeapon_HeatSeeker || Player::lastWeapon[playerId] == PlayerWeapon_Camera)
        {
            Player::lastPlayerAimSyncData[playerId].AimZ = Player::lastPlayerAimSyncData[playerId].CamFrontVector[2];

            if (Player::lastPlayerAimSyncData[playerId].AimZ > 1.0f)
            {
                Player::lastPlayerAimSyncData[playerId].AimZ = 1.0f;
            }
            else if (Player::lastPlayerAimSyncData[playerId].AimZ < -1.0f)
            {
                Player::lastPlayerAimSyncData[playerId].AimZ = -1.0f;
            }
        }

        if (Player::infiniteAmmo[playerId])
        {
            Player::lastPlayerAimSyncData[playerId].CamZoom = 2;
        }

        bs.reset();
        Player::lastPlayerAimSyncData[playerId].write(bs);
    }

    Player::lastUpdateTick[playerId] = SkyComponent::getCore()->getTickCount();
    Player::lastSyncPacket[playerId] = SyncTypes::E_AIM_SYNC;

    return true;
}

bool VehicleSync::received(IPlayer &peer, NetworkBitStream &bs)
{
    int playerId = peer.getID();

    if (Player::GetSyncFrozenState(playerId, SyncTypes::E_VEHICLE_SYNC) || Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
    {
        bs.reset();
        Player::lastVehicleSyncData[playerId].write(bs);
    }
    else
    {
        Player::lastVehicleSyncData[playerId].read(bs);

        if (Player::lastVehicleSyncData[playerId].WeaponID > PlayerWeapon::PlayerWeapon_Parachute || (Player::lastVehicleSyncData[playerId].WeaponID > PlayerWeapon::PlayerWeapon_Moltov && Player::lastVehicleSyncData[playerId].WeaponID < PlayerWeapon::PlayerWeapon_Colt45))
        {
            Player::lastVehicleSyncData[playerId].WeaponID = PlayerWeapon::PlayerWeapon_Fist;
        }

        if (Player::fakeHealth[playerId] != 255)
        {
            Player::lastVehicleSyncData[playerId].PlayerHealthArmour[0] = Player::fakeHealth[playerId];
        }

        if (Player::fakeArmour[playerId] != 255)
        {
            Player::lastVehicleSyncData[playerId].PlayerHealthArmour[1] = Player::fakeArmour[playerId];
        }

        bs.reset();
        Player::lastVehicleSyncData[playerId].write(bs);
    }

    Player::lastUpdateTick[playerId] = SkyComponent::getCore()->getTickCount();
    Player::lastSyncPacket[playerId] = SyncTypes::E_VEHICLE_SYNC;

    return true;
}

bool PassengerSync::received(IPlayer &peer, NetworkBitStream &bs)
{
    int playerId = peer.getID();

    if (Player::GetSyncFrozenState(playerId, SyncTypes::E_PASSENGER_SYNC) || Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
    {
        bs.reset();
        Player::lastPlayerPassengerSyncData[playerId].write(bs);
    }
    else
    {
        Player::lastPlayerPassengerSyncData[playerId].read(bs);

        if (Player::lastPlayerPassengerSyncData[playerId].WeaponID > PlayerWeapon::PlayerWeapon_Parachute || (Player::lastPlayerPassengerSyncData[playerId].WeaponID > PlayerWeapon::PlayerWeapon_Moltov && Player::lastPlayerPassengerSyncData[playerId].WeaponID < PlayerWeapon::PlayerWeapon_Colt45))
        {
            Player::lastPlayerPassengerSyncData[playerId].WeaponID = PlayerWeapon::PlayerWeapon_Fist;
        }

        if (Player::fakeHealth[playerId] != 255)
        {
            Player::lastPlayerPassengerSyncData[playerId].HealthArmour[0] = Player::fakeHealth[playerId];
        }

        if (Player::fakeArmour[playerId] != 255)
        {
            Player::lastPlayerPassengerSyncData[playerId].HealthArmour[1] = Player::fakeArmour[playerId];
        }

        bs.reset();
        Player::lastPlayerPassengerSyncData[playerId].write(bs);
    }

    Player::lastUpdateTick[playerId] = SkyComponent::getCore()->getTickCount();
    Player::lastSyncPacket[playerId] = SyncTypes::E_PASSENGER_SYNC;

    return true;
}

bool SpectatorSync::received(IPlayer &peer, NetworkBitStream &bs)
{
    int playerId = peer.getID();

    if (Player::GetSyncFrozenState(playerId, SyncTypes::E_PASSENGER_SYNC) || Player::GetSyncFrozenState(playerId, SyncTypes::E_ALL_SYNC))
    {
        bs.reset();
        Player::lastPlayerPassengerSyncData[playerId].write(bs);
    }
    else
    {
        Player::lastPlayerPassengerSyncData[playerId].read(bs);
    }

    Player::lastUpdateTick[playerId] = SkyComponent::getCore()->getTickCount();
    Player::lastSyncPacket[playerId] = SyncTypes::E_SPECTATING_SYNC;

    return true;
}

bool TrailerSync::received(IPlayer &peer, NetworkBitStream &bs)
{
    Player::lastUpdateTick[peer.getID()] = SkyComponent::getCore()->getTickCount();
    return true;
}