#define _____開發環境說明
/*
  安裝STM32CubeProgrammer 
  網址連結: https://www.st.com/en/development-tools/stm32cubeprog.html#get-software
  
  Arduino IDE 開發環境安裝
    1.喜好設定 > 其他開發版管理員網址 > 添加  https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json  保存
    
    2.開發版管理員 搜尋 "STM32" 安裝 "STM32 MCU based boards"

    3.開發版選擇 "STM32 MCU based boards" > Generic STM32F4 series

        STM32F401 BlackPill - USB CDC 測試程式
        Tools 設定要選：
          - Board: Generic STM32F4 series
          - Board part number: BlackPill F401CC
          - Upload method: STM32CubeProgrammer (DFU)
          - USB support (if available): CDC (generic 'Serial')
          - U(S)ART support: Enabled (generic 'Serial')
          - USB speed: Low/Full Speed
  
  燒錄: Arduino 燒錄完畢 > 開發版連接ST Link 連接電腦後 > 開啟STM32CubeProgrammer > 右邊控制欄上方 
  下拉式選單選擇 ST Link > 連線 > 打開文件 > 選擇bin檔案 路徑在 C:\Users\使用者名稱\AppData\Local\arduino\sketches\文件名稱\xxxxxx.bin > 下載 > 彈出視窗顯示 "文件下載完成" 即燒錄完成


*/

#define _____韌體說明
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						韌體說明							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//	溫度模組(Temperature Module)
//	應用於Opentrons OT-2

#define _____功能開關
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						功能開關							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//	
//	啟用:取消"//"註解 禁用:添加"//"註解
#define GPIO
#define RTOS
#define TEMP
#define GCODE
#define MIRROR


#define _____匯入函式
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						匯入函式							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//
//
// 匯入STM32FreeRTOS
#ifdef RTOS
#include <STM32FreeRTOS.h>

#endif




#define _____常數定義
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						常數定義							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//

// 強制定義 Serial2 使用 USART2（PA3 = RX，PA2 = TX）
HardwareSerial Serial2(USART2);  // 實際會自動綁定到 PA3(RX) / PA2(TX)

//	RGB燈號
//
#define LED_R		PB13
#define	LED_G		PB14
#define LED_B		PB15

//
//	溫度感應器
//
#ifdef TEMP
#define	TEMP1		PA8		// NTC100K熱敏電阻 溫度台溫度計
#define TEMP2		PA9		// NTC100K熱敏電阻 散熱風扇
#endif
//
//	BTS7960 H 橋控制腳位定義
//
#define	C_EN		PA10	// 致冷 R_EN
#define	C_PWM		PB9		// 致冷 R_PWM
#define	C_IS		PB8		// 致冷 R_IS
#define	H_EN		PA15	// 致熱 L_EN
#define	H_PWM		PB3		// 致熱 L_PWM
#define	H_IS		PB4		// 致冷 L_IS
//
//	風扇控制腳位定義
//
#define F_SIG		PB5 	// 風扇控制

//
//	OLED顯示器
//
#define OLED_SCL	PB6		// SCL
#define	OLED_SDA	PB7		// SDA

//
//	PID數值
//



#define _____腳位定義
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						腳位定義							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//
//	函式用途: 腳位模式定義
#ifdef GPIO
void IO_MODE() {
//輸出
	pinMode(C_EN , OUTPUT);
	pinMode(H_EN , OUTPUT);
    pinMode(LED_R , OUTPUT);
	pinMode(LED_G , OUTPUT);
	pinMode(LED_B , OUTPUT);
	pinMode(C_PWM , OUTPUT);
	pinMode(H_PWM , OUTPUT);
	pinMode(F_SIG , OUTPUT);
//輸入	
	pinMode(C_IS , INPUT);
	pinMode(H_IS , INPUT);
	pinMode(TEMP1 , INPUT);
	pinMode(TEMP2 , INPUT);
}
#endif


//	// USB 裝置識別資訊
#if 1
#define USBD_PRODUCT_STRING       "Opentrons TempDeck Gen1"
#define USBD_MANUFACTURER_STRING  "Opentrons"
#define USBD_SERIAL_STRING        "TM12345678901234"
#define USBD_VID                  0x0403
#define USBD_PID                  0x6001

// ======== 模組身分（可調）========
static const char FW_NAME[]    = "opentrons-tempdeck";		//韌體名稱
static const char FW_VERSION[] = "3.0.0";					//韌體版本
static const char MODEL_STR[]  = "tempdeck-v3";
static const char SERIAL_NO[]  = "TD3A01";
static const char HW_NAME[]    = "tempdeck-gen2";   		// 硬體名稱 例 "tempdeck-gen3"
static const char SERIAL_SN[]  = "B001AQF8A";				// 模組序號（）

#endif

#ifdef MIRROR
// ======== 鏡射設定：Serial2 on PA10/PA9 ========
#define MIRROR_UART   Serial2
#define MIRROR_BAUD   115200
#define MIRROR_RX_PIN PA3   	// MCU 接收腳（接 USB-TTL 的 TX）
#define MIRROR_TX_PIN PA2    	// MCU 送出腳（接 USB-TTL 的 RX)
#endif


#define _____全域變數	
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						全域變數							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//


#ifdef TEMP
// ======= TEMP1 腳位實測溫度函式 =======
float readNTCTempC() {
  const int analogPin = TEMP1;
  const float beta = 3950.0;
  const float r0 = 100000.0;       // NTC 在 25°C 的阻值（100kΩ）
  const float t0 = 298.15;         // 25°C = 298.15K
  const float seriesResistor = 100000.0;  // 分壓電阻

  int adc = analogRead(analogPin);
  if (adc == 0) return -273.15;  // 保護：避免除以零

  float v = adc * 3.3 / 4095.0;
  float r = (3.3 * seriesResistor / v) - seriesResistor;

  float tempK = 1.0 / (1.0 / t0 + log(r / r0) / beta);
  float tempC = tempK - 273.15;

  return tempC;
}
#endif

/*  Tempdeck_Handshake_Mirror.ino
 *  功能：實作 OT-2 掃描握手（M115 回覆）＋鏡射日誌到 Serial1
 *  板子：STM32F401 BlackPill（Arduino_Core_STM32）
 *  Tools -> USB support: CDC (generic 'Serial')
 *  Serial (OT-2/USB CDC) 與 Serial1（鏡射）皆 115200 8-N-1
 */


#ifndef ARDUINO
#error "Please compile with Arduino IDE / Arduino_Core_STM32."
#endif

//Temperature Module GEN2

#ifdef MIRROR
//訊號鏡射
// ---------- 小工具：鏡射 ----------
static inline void logRX(const char* s) {
  MIRROR_UART.print("RX: ");
  MIRROR_UART.println(s);
}
static inline void logTX(const char* s) {
  MIRROR_UART.print("TX: ");
  MIRROR_UART.println(s);
}

// 單行回覆（自動鏡射）
static void sendLine(const char* s) {
  Serial.print(s);
  Serial.print('\n');
  Serial.flush();
  logTX(s);
}
#endif


// printf 版回覆（自動鏡射）
static void sendReplyf(const char* fmt, ...) {
  char out[160];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(out, sizeof(out), fmt, ap);
  va_end(ap);
  sendLine(out);
}

#ifdef GCODE

// ---- NEW: 產生要放進回報的序號字串 ----
static const char* getSerialForReport() {
  static char clean[32];
  size_t j = 0;
  for (size_t i = 0; i < sizeof(SERIAL_SN) && SERIAL_SN[i] != '\0'; ++i) 
  {
    unsigned char c = (unsigned char)SERIAL_SN[i];
    if (c == 0xFF) break;              // 原廠測試：遇到 0xFF 視為無效並截斷

	if (isprint(c)) 
	{
      if (j < sizeof(clean) - 1) clean[j++] = (char)c;
    }
  }
  clean[j] = '\0';
  if (j == 0) return "EMPTYSN";        // 原廠測試：空序號用 EMPTYSN
  return clean;
}

// ======== 簡易狀態（供 M105/M18 示意，不影響握手）========
static float currentTempC = 25.0f;
static float targetTempC  = 0.0f;
enum class Status : uint8_t { Idle, Heating, Cooling, Holding };
static Status status = Status::Idle;

// ======== 行讀取緩衝 ========
static char lineBuf[128];
static size_t lineIdx = 0;

// 跳過空白
static char* skipSpaces(char* p) { while (*p==' '||*p=='\t') ++p; return p; }

// 解析 M 指令號碼（"M115"->115）
static int parseGCodeNumber(const char* p) {
  int n = 0;
  while (*p) {
    if (*p >= '0' && *p <= '9') n = n*10 + (*p - '0');
    else if (*p==' '||*p=='\t'||*p=='\r'||*p=='\n') break;
    ++p;
  }
  return n;
}

// 取得參數 S 的數值（"M104 S50"->50.0），找不到回 NAN
static float findParamS(const char* p) {
  while (*p) {
    if (*p=='S' || *p=='s') {
      ++p;
      if (*p==' '||*p=='\t') p = skipSpaces((char*)p);
      return atof(p);
    }
    ++p;
  }
  return NAN;
}

#define ________G_code
/*
	握手G-code
*/
static void handleM115() {
  // 一行回完握手資訊


	// 目標格式： "M115 FW:<fw> HW:<hw> SerialNo:<sn> OK\n"
  sendReplyf("M115 FW:%s HW:%s SerialNo:%s OK", FW_NAME, HW_NAME, getSerialForReport());

}

/*
	讀取溫度 G-code
*/
static void handleM105() {
  const char* st =
      (status==Status::Idle)    ? "idle" :
      (status==Status::Heating) ? "heating" :
      (status==Status::Cooling) ? "cooling" : "holding";
  sendReplyf("ok T:%.1f target:%.1f status:%s", currentTempC, targetTempC, st);
}

/*
	設定溫度 G-code
*/

static void handleM104(char* cmd) {
  float s = findParamS(cmd);
  if (!isnan(s)) {
    targetTempC = s;
    if (fabsf(currentTempC - targetTempC) < 0.3f) status = Status::Holding;
    else if (currentTempC < targetTempC)           status = Status::Heating;
    else                                           status = Status::Cooling;
    sendLine("ok");
  } else {
    sendLine("error: missing S parameter");
  }
}


/*
	停用 G-code
*/

static void handleM18() {
  targetTempC = 0.0f;
  status = Status::Idle;
  sendLine("ok status:idle");
}


static void handleLine(char* line) {
#ifdef MIRROR
  logRX(line);                     // ← 鏡射收到的整行
#endif  
  char* p = skipSpaces(line);
  char c0 = (*p>='a'&&*p<='z') ? (*p-32) : *p;  // to upper for command head

  if (c0 == 'M') {
    int code = parseGCodeNumber(p + 1);
    switch (code) {
      case 115: handleM115();          break; // 握手
      case 105: handleM105();          break; // 讀溫（stub）
      case 104: handleM104(p);         break; // 設溫（stub）
      case 18:  handleM18();           break; // 停用（stub）
      default:  sendLine("error: unsupported command"); break;
    }
  } else {
    sendLine("error: unsupported command");
  }
}

#endif  //GCODE  240行

void setup() {
  
  Serial.begin(115200);		// 主串列：供 OT-2（USB CDC）
  IO_MODE();				//	初始化GPIO
#ifdef MIRROR

  // 鏡射串列：明確綁定到 PA10/PA9，避免吃到預設替代腳
  MIRROR_UART.setRx(MIRROR_RX_PIN);
  MIRROR_UART.setTx(MIRROR_TX_PIN);
  MIRROR_UART.begin(MIRROR_BAUD);
  MIRROR_UART.println("Mirror logger started @115200 (RX=PA10, TX=PA9)");
  
#endif  //MIRROR  381行

  // 不主動吐任何字給 OT-2；等待它送 M115
}

void loop() {
  // 非阻塞讀一整行（\n 結束；忽略 \r）
  // 讀取 USB訊號 
  while (Serial.available() > 0) {
    char ch = (char)Serial.read();
    if (ch == '\r') continue;
    if (ch == '\n') {
      lineBuf[lineIdx] = '\0';
      if (lineIdx > 0) handleLine(lineBuf);
      lineIdx = 0;
    } else {
      if (lineIdx < sizeof(lineBuf) - 1) lineBuf[lineIdx++] = ch;
      else lineIdx = 0; // overflow，丟棄本行
    }
  }

  
#if 0
#ifdef TEMP_TEST  
  // ---- 溫度趨近模擬（示意）----
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  if (now - lastMs >= 200) {
    lastMs = now;
    float err = targetTempC - currentTempC;
    currentTempC += constrain(err * 0.05f, -0.2f, 0.2f);
    if (fabsf(err) < 0.3f && status != Status::Idle) status = Status::Holding;
  }
#endif   //TEMP_TEST
#endif   // 0
#ifdef TEMP
	// ---- 溫度實測 ----
	static uint32_t lastMs = 0;
	uint32_t now = millis();
	if (now - lastMs >= 500) {
	  lastMs = now;
	
	  float newTemp = readNTCTempC();  // 讀取 TEMP1 腳位實測溫度
	
	  if (!isnan(newTemp) && newTemp > -50 && newTemp < 150) {
		currentTempC = newTemp;
	
		// 狀態判斷
		float err = targetTempC - currentTempC;
		if (fabsf(err) < 0.3f && status != Status::Idle) status = Status::Holding;
		else if (currentTempC < targetTempC)			 status = Status::Heating;
		else											 status = Status::Cooling;
	  }
	}

//鏡射顯示（Debug）		
#ifdef MIRROR
	  static uint32_t lastLogMs = 0;
	  if (now - lastLogMs >= 1000) {
		lastLogMs = now;
		MIRROR_UART.print("[TEMP] ");
		MIRROR_UART.println(currentTempC, 2);
	  }
#endif		//MIRROR
#endif		//TEMP

}//loop
