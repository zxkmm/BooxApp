/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

//#include <unistd.h>
#include <stdlib.h>

#include <ZLFile.h>

#include "ZLUnixMessage.h"

ZLUnixCommunicationManager::ZLUnixCommunicationManager() {
}

void ZLUnixCommunicationManager::createInstance() {
	if (ourInstance == 0) {
		ourInstance = new ZLUnixCommunicationManager();
	}
}

fb::shared_ptr<ZLMessageOutputChannel> ZLUnixCommunicationManager::createMessageOutputChannel(const std::string &protocol, const std::string &testFile) {
	if (protocol != "execute") {
          return fb::shared_ptr<ZLMessageOutputChannel>();
	}

	if (!testFile.empty() && !ZLFile(testFile).exists()) {
          return fb::shared_ptr<ZLMessageOutputChannel>();
	}

	return fb::shared_ptr<ZLMessageOutputChannel>(
            new ZLUnixExecMessageOutputChannel());
}

fb::shared_ptr<ZLMessageSender> ZLUnixExecMessageOutputChannel::createSender(const ZLCommunicationManager::Data &data) {
	ZLCommunicationManager::Data::const_iterator it = data.find("command");
	if (it == data.end()) {
          return fb::shared_ptr<ZLMessageSender>();
	}
	const std::string &command = it->second;
	return (!command.empty()) ?
            fb::shared_ptr<ZLMessageSender>(new ZLUnixExecMessageSender(command))
            : fb::shared_ptr<ZLMessageSender>();
}

ZLUnixExecMessageSender::ZLUnixExecMessageSender(const std::string &command) : myCommand(command) {
}

void ZLUnixExecMessageSender::sendStringMessage(const std::string &message) {
#ifndef _WINDOWS
    if (fork() == 0) {
		std::string escapedMessage = message;
		int index = 0;
		while (true) {
			index = escapedMessage.find('&', index);
			if (index == -1) {
				break;
			}
			escapedMessage.insert(index, "\\");
			index += 2;
		}
		index = 0;
		while (true) {
			index = escapedMessage.find(' ', index);
			if (index == -1) {
				break;
			}
			escapedMessage.insert(index, "\\");
			index += 2;
		}

		std::string command = myCommand;
		index = command.find("%1");
		if (index >= 0) {
			command = command.substr(0, index) + escapedMessage + command.substr(index + 2);
		}
		system(command.c_str());
		exit(0);
	}
#endif
}
