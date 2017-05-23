void ProtocolGame::sendBasicData()
{
    NetworkMessage msg;
    msg.addByte(0x9F);
    msg.addByte(player->isPremium() ? 0x01 : 0x00);
    msg.add<uint32_t>(player->isPremium() ? time(nullptr) + (player->premiumDays * 86400) : time(nullptr) + (player->premiumDays * 86400));
    msg.addByte(player->getVocation()->getClientId());
	// Prey System
	if (player->getVocation()->getId() == 0) {
		msg.addByte(0);
	}
	else {
		msg.addByte(1); // has reached Main (allow player to open Prey window)
	}
	msg.add<uint16_t>(0xFF); // number of known spells
	for (uint8_t spellId = 0x00; spellId < 0xFF; spellId++) {
		msg.addByte(spellId);
	}
	writeToOutputBuffer(msg);
}
