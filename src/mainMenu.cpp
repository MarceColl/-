#include "mainMenu.h"

uint8_t pageIndex = 0;
uint8_t cameraY = 0;
uint16_t directoryCount = 0;
String directories[100];
String BinaryFiles[100];
uint16_t binaryCount = 0;

void listDirectories(const char * dirname) {
	directoryCount = 0;
	Serial.printf("Listing directory: %s\n", dirname);

	File root = SD.open(dirname);

	if (!root) {
		Serial.println(F("Failed to open directory"));
		return;
	}

	if (!root.isDirectory()) {
		Serial.println(F("Not a directory"));
		return;
	}

	int counter = 0;

	File file = root.openNextFile();
	while (file) {

		if (file.isDirectory()) {
			String Name(file.name());
			Serial.println(Name);
			if(Name != "/Images\0" && Name != "/Music\0" && Name != "/Video\0"
			 && Name != "/System Volume Information\0" && SD.exists(String(Name + "/" + Name + ".BIN")))
			{
				Serial.println(Name);
				directories[directoryCount] = Name.substring(1);
				counter++;
				directoryCount++;
			}
		  }
		file = root.openNextFile();
	}
}

void listBinaries(const char * dirname, uint8_t levels) {
	binaryCount = 0;
	Serial.printf("Listing directory: %s\n", dirname);

	File root = SD.open(dirname);
	if (!root) {
		Serial.println(F("Failed to open directory"));
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(F("Not a directory"));
		return;

	}
	int counter = 1;

	File file = root.openNextFile();
	while (file) {
		String Name(file.name());
		Serial.println(Name);
		if (Name.endsWith(F(".BIN")) || Name.endsWith(F(".bin")))
		{
			Serial.print(counter);
			Serial.print(F(".   "));
			Serial.println(Name);
			BinaryFiles[counter - 1] = Name;
			counter++;
			binaryCount++;
		}
		file = root.openNextFile();
	}
}

int16_t scrollingMainMenu(uint16_t _cursor) {
	bool SDinserted = mp.SDinsertedFlag;
	bool cursorState = 0;
	uint16_t index = 0;
	uint8_t cursorX = 0;
	uint8_t cursorY = 0;
	uint8_t elements = APP_COUNT + directoryCount;
	uint8_t x_elements = 3;
	uint8_t y_elements = ceil((float)elements/x_elements);
	
	uint8_t pageNumber;
	if(elements < 6) {
		pageNumber = 0;
	} else {
		pageNumber = ceil((float)(elements - 6)/3);
	}

	cursorY = int(_cursor / x_elements);
	cursorX = _cursor % x_elements;
	Serial.println(cursorY);
	uint32_t elapsedMillis = millis();
	uint32_t elapsedMillis2 = millis();
	bool newScreen = 1;
	mp.display.fillScreen(TFT_BLACK);
	Serial.println(mp.buttons.timeHeld(BTN_A));
	// while(!mp.update());
	while (1) {
		mp.display.fillRect(0,0,mp.display.width(), 14, TFT_BLACK);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);

		// Draw the icons
		if(newScreen) {
			mp.display.fillScreen(TFT_BLACK);
			for (int i = 0; i < APP_COUNT; i++) {
				uint8_t tempX = i%x_elements;
				uint8_t tempY = i/x_elements;
				switch (pageIndex * 3 + i) {
					case 0:
						mp.display.drawIcon(bigPhone, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 1:
						mp.display.drawIcon(bigContacts, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 2:
						mp.display.drawIcon(bigMessages, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 3:
						mp.display.drawIcon(bigSettings, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 4:
						mp.display.drawIcon(clock_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 5:
						mp.display.drawIcon(flash_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					default:
						if(pageIndex * 3 + i < elements)
						{
							Serial.println(directories[pageIndex * 3 + i - APP_COUNT]);
							delay(5);
							if(SD.exists(String("/" + directories[pageIndex * 3 + i - APP_COUNT] + "/icon.bmp"))) {
								mp.display.drawBmp(String("/" + directories[pageIndex * 3 + i - APP_COUNT] + "/icon.bmp"), 4 + tempX * 52, 18 + tempY * 56, 2);
							} else {
								mp.display.drawIcon(defaultIcon, 4 + tempX * 52, 18 + tempY * 56, width, bigIconHeight, 2);
							}
						}
						break;
				}
			}
			newScreen = 0;
		}

		mp.display.fillRect(0, 0, 160, 12, TFT_BLACK);

		while (cursorY*x_elements + cursorX >= elements) {
			cursorX--;
		}

		index = cursorY * x_elements + cursorX;
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
		if(index < 6) {
			mp.display.print(titles[index]);
		} else {
			mp.display.print(directories[index-6]);
		}

		if (millis() - elapsedMillis >= 250) {
			elapsedMillis = millis();
			cursorState = !cursorState;
		}

		if (millis() - elapsedMillis2 >= 100) {
			elapsedMillis2 = millis();
		}

		mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, cursorState ? TFT_RED : TFT_BLACK);
		mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, cursorState ? TFT_RED : TFT_BLACK);

		///////////////////////////////////////
		//////Checking for button input////////
		///////////////////////////////////////
		if (mp.buttons.released(BTN_A)) {
			mp.buttons.update();
			return cursorY * x_elements + cursorX;  //returns index of selected icon
		}

		if (mp.buttons.pressed(BTN_UP)) {
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			mp.osc->note(75, 0.05);
			mp.osc->play();
			mp.leds[0] = CRGB::Blue;
			mp.leds[7] = CRGB::Blue;
			FastLED.clear();
			while(!mp.update());

			if (cursorY == 0) {
				newScreen = 1;
				cursorY = y_elements-1;

				if(pageNumber > 0) {
					cameraY = 1;
					pageIndex = pageNumber;
				}
			}
			else if(cameraY % 2 == 1) {
				cursorY--;
				cameraY = 0;
			}
			else if(cameraY % 2 == 0 && pageIndex > 0) {
				newScreen = 1;
				cameraY = 0;
				cursorY--;
				pageIndex--;
			}
			elapsedMillis = millis();
			cursorState = 1;
		}

		if (mp.buttons.pressed(BTN_DOWN)) {
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			mp.osc->note(75, 0.05);
			mp.osc->play();
			mp.leds[3] = CRGB::Blue;
			mp.leds[4] = CRGB::Blue;
			FastLED.clear();
			while(!mp.update());

			if (cursorY == y_elements - 1) {
				newScreen = 1;
				cursorY = 0;
				pageIndex = 0;
				cameraY = 0;
			} else if(cameraY % 2 == 0) {
				cursorY++;
				cameraY++;
			} else if (cameraY % 2 == 1 && pageIndex < pageNumber) {
				newScreen = 1;
				cameraY = 1;
				cursorY++;
				pageIndex++;
			}

			elapsedMillis = millis();
			cursorState = 1;
		}

		if (mp.buttons.pressed(BTN_LEFT)) {
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			mp.osc->note(75, 0.05);
			mp.osc->play();
			mp.leds[6] = CRGB::Blue;
			mp.leds[5] = CRGB::Blue;
			FastLED.clear();
			while(!mp.update());

			if (cursorX == 0) {
				cursorX = x_elements - 1;
			} else {
				cursorX--;
			}

			elapsedMillis = millis();
			cursorState = 1;
		}

		if (mp.buttons.pressed(BTN_RIGHT)) {
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			mp.osc->note(75, 0.05);
			mp.osc->play();
			mp.leds[1] = CRGB::Blue;
			mp.leds[2] = CRGB::Blue;
			FastLED.clear();
			while(!mp.update());

			if (cursorX == x_elements - 1) {
				cursorX = 0;
			}
			else
				cursorX++;
			elapsedMillis = millis();
			cursorState = 1;
		}

		if (mp.buttons.released(BTN_B)) {
			mp.leds[0] = CRGB::Red;
			mp.leds[7] = CRGB::Red;
			FastLED.show();
			FastLED.clear();
			while(!mp.update());
			pageIndex = 0;
			cameraY = 0;
			return -2;
		}

		if(SDinserted != mp.SDinsertedFlag) {
			SDinserted = mp.SDinsertedFlag;
			pageIndex = 0;
			cameraY = 0;
			return -1;
		}
		if(mp.exitedLockscreen) {
			mp.display.fillScreen(TFT_BLACK);
			for (int i = 0; i < APP_COUNT; i++)
			{
				uint8_t tempX = i%x_elements;
				uint8_t tempY = i/x_elements;
				switch (pageIndex * 3 + i) {
					case 0:
						mp.display.drawIcon(bigPhone, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 1:
						mp.display.drawIcon(bigContacts, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 2:
						mp.display.drawIcon(bigMessages, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 3:
						mp.display.drawIcon(bigSettings, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 4:
						mp.display.drawIcon(clock_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 5:
						mp.display.drawIcon(flash_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					default:
						if(pageIndex * 3 + i < elements) {
							Serial.println(directories[pageIndex * 3 + i - APP_COUNT]);
							delay(5);
							if(SD.exists(String("/" + directories[pageIndex * 3 + i - APP_COUNT] + "/icon.bmp"))) {
								mp.display.drawBmp(String("/" + directories[pageIndex * 3 + i - APP_COUNT] + "/icon.bmp"), 4 + tempX * 52, 18 + tempY * 56, 2);
							} else {
								mp.display.drawIcon(defaultIcon, 4 + tempX * 52, 18 + tempY * 56, width, bigIconHeight, 2);
							}
						}
						break;
				}
			}
		}
		mp.update();
	}
}

void mainMenu() {
	while(1)
	{
		int16_t index = 0;
		Serial.println(F("entered main menu"));
		mp.dataRefreshFlag = 0;
		listDirectories("/");
		while (1)
		{
			index = scrollingMainMenu(index);
			Serial.println(index);
			delay(5);
			if (index == -2) {
				Serial.println(F("pressed"));
				mp.lockscreen();
				break;
			}
			else if(index == -1) {
				break;
			} else if(index < APP_COUNT) {
				if (index == APP_MESSAGES) {
					if(mp.SDinsertedFlag) {
						while(!mp.update());
						messagesApp();
					} else if(!mp.SDinsertedFlag) {
						mp.display.fillScreen(TFT_BLACK);
						mp.display.setTextColor(TFT_WHITE);
						mp.display.setCursor(0, mp.display.height()/2 - 20);
						mp.display.setTextFont(2);
						mp.display.printCenter(F("Can't access SMS!"));
						mp.display.setCursor(0, mp.display.height()/2);
						mp.display.printCenter(F("SD card missing"));
						uint32_t tempMillis = millis();

						while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B)) {
							mp.update();
						}

						while(!mp.update());
					}
				} else if (index == APP_PHONE) {
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setTextColor(TFT_WHITE);
					phoneApp();
				} else if (index == APP_CONTACTS) {
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setTextColor(TFT_WHITE);
					if(mp.SDinsertedFlag) {
						mp.display.fillScreen(TFT_BLACK);
						mp.display.setCursor(0,mp.display.height()/2 - 16);
						mp.display.printCenter(F("Loading contacts..."));
						contactsApp(false);
					} else if(!mp.SDinsertedFlag) {
						mp.display.setCursor(0, mp.display.height()/2 - 20);
						mp.display.setTextFont(2);
						mp.display.printCenter(F("No SD inserted!"));
						mp.display.setCursor(0, mp.display.height()/2);
						mp.display.printCenter(F("Insert SD and reset"));
						uint32_t tempMillis = millis();

						while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B)) {
							mp.update();
						}

						while(!mp.update());
					}
				} else if (index == APP_SETTINGS) {
					if(settingsApp()) {
						pageIndex = 0;
						cameraY = 0;
						return;
					}
				} else if(index == APP_CLOCK) {
					clockApp();
				} else if(index == APP_FLASHLIGHT) {
					flashlightApp();
				}
			} else {
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setCursor(0,mp.display.height() / 2 - 16);
				mp.display.printCenter(F("LOADING NOW..."));
				while(!mp.update());

				String foo = directories[index - 6]; // 6 is the number of preloaded apps
				initWavLib();
				mp.updateFromFS(String("/" + foo + "/" + foo + ".bin"));
			}

			mp.update();
		}
	}
}
