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




//	// USB 裝置識別資訊
#if 1
#define USBD_PRODUCT_STRING       "Opentrons TempDeck Gen1"
#define USBD_MANUFACTURER_STRING  "Opentrons"
#define USBD_SERIAL_STRING        "TM12345678901234"
#define USBD_VID                  0x0403
#define USBD_PID                  0x6001
#define SN                        

// 啟用 CDC（虛擬序列埠）
#define USBCON
#define USBD_USE_CDC
#endif

#define _____匯入函式
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						匯入函式							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//
//
// 匯入STM32FreeRTOS
#include <STM32FreeRTOS.h>
#if 0
#define STR_(x) #x
#define STR(x) STR_(x)
#warning USBD_MANUFACTURER_STRING=STR(USBD_MANUFACTURER_STRING)
#warning USBD_PRODUCT_STRING=STR(USBD_PRODUCT_STRING)
#endif

#define _____常數定義
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						常數定義							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//
//	RGB燈號
//
#define LED_R	26
#define	LED_G	27
#define LED_B	28

//
//	溫度感應器
//
#define	TEMP1	29	// NTC100K熱敏電阻 溫度台溫度計
#define TEMP2	30	// NTC100K熱敏電阻 散熱風扇

//
//	BTS7960 H 橋控制腳位定義
//
#define	C_EN	31	// 致冷 R_EN
#define	C_PWM	32	// 致冷 R_PWM
#define	C_IS	33	// 致冷 R_IS
#define	H_EN	38	// 致熱 L_EN
#define	H_PWM	39	// 致熱 L_PWM
#define	H_IS	40	// 致冷 L_IS

//
//	風扇控制腳位定義
//
#define F_SIG	41 	// 風扇控制

//
//	OLED顯示器
//
#define OLED_SCL		42	
#define	OLED_SDA		43

//
//	PID數值
//




#define _____全域變數	
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						全域變數							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//

/*  Tempdeck_Handshake_Mirror.ino
 *  功能：實作 OT-2 掃描握手（M115 回覆）＋鏡射日誌到 Serial1
 *  板子：STM32F401 BlackPill（Arduino_Core_STM32）
 *  Tools -> USB support: CDC (generic 'Serial')
 *  Serial (OT-2/USB CDC) 與 Serial1（鏡射）皆 115200 8-N-1
 */

#if 1
#ifndef ARDUINO
#error "Please compile with Arduino IDE / Arduino_Core_STM32."
#endif

// ======== 模組身分（可調）========
static const char FW_NAME[]    = "opentrons-tempdeck";
static const char FW_VERSION[] = "3.0.0";
static const char MODEL_STR[]  = "tempdeck-v3";
static const char SERIAL_NO[]  = "TD3A01";
static const char HW_NAME[]    = "tempdeck-gen2";   // NEW: 例 "tempdeck-gen3"
static const char SERIAL_SN[]  = "B001AQF8A";          // NEW: 你的序號（可換成讀EEPROM）

//Temperature Module GEN1


// ---- NEW: 產生要放進回報的序號字串 ----
static const char* getSerialForReport() {
  static char clean[32];
  size_t j = 0;
  for (size_t i = 0; i < sizeof(SERIAL_SN) && SERIAL_SN[i] != '\0'; ++i) {
    unsigned char c = (unsigned char)SERIAL_SN[i];
    if (c == 0xFF) break;              // 原廠測試：遇到 0xFF 視為無效並截斷
    if (isprint(c)) {
      if (j < sizeof(clean) - 1) clean[j++] = (char)c;
    }
  }
  clean[j] = '\0';
  if (j == 0) return "EMPTYSN";        // 原廠測試：空序號用 EMPTYSN
  return clean;
}


// ======== 鏡射設定：Serial1 on PA10/PA9 ========
#define MIRROR_UART   Serial1
#define MIRROR_BAUD   115200
#define MIRROR_RX_PIN PA10   // MCU 接收腳（接 USB-TTL 的 TX）
#define MIRROR_TX_PIN PA9    // MCU 送出腳（接 USB-TTL 的 RX)

// ======== 簡易狀態（供 M105/M18 示意，不影響握手）========
static float currentTempC = 25.0f;
static float targetTempC  = 0.0f;
enum class Status : uint8_t { Idle, Heating, Cooling, Holding };
static Status status = Status::Idle;

// ======== 行讀取緩衝 ========
static char lineBuf[128];
static size_t lineIdx = 0;

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

// printf 版回覆（自動鏡射）
static void sendReplyf(const char* fmt, ...) {
  char out[160];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(out, sizeof(out), fmt, ap);
  va_end(ap);
  sendLine(out);
}

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

static void handleM115() {
  // 一行回完握手資訊
 #if 0 
  sendReplyf("ok FIRMWARE_NAME:%s FIRMWARE_VERSION:%s MODEL:%s SERIAL:%s",
             FW_NAME, FW_VERSION, MODEL_STR, SERIAL_NO);
#endif

	// 目標格式： "M115 FW:<fw> HW:<hw> SerialNo:<sn> OK\n"
  sendReplyf("M115 FW:%s HW:%s SerialNo:%s OK", FW_NAME, HW_NAME, getSerialForReport());

}

static void handleM105() {
  const char* st =
      (status==Status::Idle)    ? "idle" :
      (status==Status::Heating) ? "heating" :
      (status==Status::Cooling) ? "cooling" : "holding";
  sendReplyf("ok T:%.1f target:%.1f status:%s", currentTempC, targetTempC, st);
}

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

static void handleM18() {
  targetTempC = 0.0f;
  status = Status::Idle;
  sendLine("ok status:idle");
}

static void handleLine(char* line) {
  logRX(line);                     // ← 鏡射收到的整行
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

void setup() {
  // 主串列：供 OT-2（USB CDC）
  Serial.begin(115200);

  // 鏡射串列：明確綁定到 PA10/PA9，避免吃到預設替代腳
  MIRROR_UART.setRx(MIRROR_RX_PIN);
  MIRROR_UART.setTx(MIRROR_TX_PIN);
  MIRROR_UART.begin(MIRROR_BAUD);
  MIRROR_UART.println("Mirror logger started @115200 (RX=PA10, TX=PA9)");

  // 不主動吐任何字給 OT-2；等待它送 M115
}

void loop() {
  // 非阻塞讀一整行（\n 結束；忽略 \r）
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

  // ---- 溫度趨近模擬（示意）----
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  if (now - lastMs >= 200) {
    lastMs = now;
    float err = targetTempC - currentTempC;
    currentTempC += constrain(err * 0.05f, -0.2f, 0.2f);
    if (fabsf(err) < 0.3f && status != Status::Idle) status = Status::Holding;
  }
}
#endif
#if 0
// ======= 可調參數 =======
#define USE_FAKE_TEMP     1          // 1=用假溫度; 0=改為你的ADC/感測器讀值
static const uint32_t SERIAL_BAUD   = 115200;
static const float    DEFAULT_T_C   = 25.0f;
static const float    LIMIT_MIN_T   = -20.0f;   // 合理邊界，避免亂值
static const float    LIMIT_MAX_T   = 300.0f;


// ======= 共用狀態 (以 Mutex 保護) =======
static float g_targetC  = DEFAULT_T_C;
static float g_currentC = 25.0f;
static SemaphoreHandle_t g_stateMutex  = nullptr;
static SemaphoreHandle_t g_serialMutex = nullptr;

#define SERIAL_LOCK()   do{ if(g_serialMutex) xSemaphoreTake(g_serialMutex, pdMS_TO_TICKS(50)); }while(0)
#define SERIAL_UNLOCK() do{ if(g_serialMutex) xSemaphoreGive(g_serialMutex); }while(0)



// ======= 工具：修剪 & 抽 S 參數 =======
static String stripCommentsAndTrim(const String& in) {
  String s = in;
  int semi = s.indexOf(';'); if (semi >= 0) s.remove(semi);
  int p1 = s.indexOf('('), p2 = s.indexOf(')');
  if (p1 >= 0 && p2 > p1) s.remove(p1, p2 - p1 + 1);
  s.trim();
  return s;
}
static bool extractFloatAfterKey(const String& line, char key, float& outVal) {
  int idx = line.indexOf(String(key));
  if (idx < 0) return false;
  idx++;
  while (idx < (int)line.length() && isspace(line[idx])) idx++;
  String num;
  while (idx < (int)line.length() && !isspace(line[idx])) num += line[idx++];
  if (num.length() == 0) return false;
  outVal = num.toFloat();
  return true;
}

// ======= 回覆（Marlin風格）=======
static void replyOk() {
  SERIAL_LOCK();
  Serial.println(F("ok"));
  SERIAL_UNLOCK();
}

static void replyTemp() {
  float cur, tgt;
  if (g_stateMutex && xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
    cur = g_currentC;
    tgt = g_targetC;
    xSemaphoreGive(g_stateMutex);
  } else {
    cur = g_currentC; tgt = g_targetC;
  }
  SERIAL_LOCK();
  Serial.print(F("ok T:")); Serial.print(cur, 2);
  Serial.print(F(" /"));    Serial.println(tgt, 2);
  SERIAL_UNLOCK();
}

// ======= G-code 任務 =======
static void TaskGcode(void* pv) {
  // 等待 Serial ready
  #if 0
  withSerial([]{
    Serial.println(F("--- G-code task started ---"));
    Serial.println(F("Supported: M104 S<temp>, M105"));
  });
  #endif
  	SERIAL_LOCK();
	Serial.println(F("--- G-code task started ---"));
	Serial.println(F("Supported: M104 S<temp>, M105"));
	SERIAL_UNLOCK();

  String line;
  line.reserve(128);

  for (;;) {
    // 非阻塞收一行
    while (Serial.available()) {
      char c = (char)Serial.read();
      if (c == '\r') continue;
      if (c == '\n') 
	  {
        String cmd = stripCommentsAndTrim(line);
        line = "";
        if (cmd.length()) 
		{
          String u = cmd; u.toUpperCase();
          if (u.startsWith(F("M104"))) 
		  {
            float s;
            if (extractFloatAfterKey(u, 'S', s)) 
			{
              // 邊界保護
              if (s < LIMIT_MIN_T) s = LIMIT_MIN_T;
              if (s > LIMIT_MAX_T) s = LIMIT_MAX_T;
              if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                g_targetC = s;
                xSemaphoreGive(g_stateMutex);
              }
            }
            replyOk();
          } else if (u.startsWith(F("M105"))) {
            replyTemp();
          } else {
            // 其他指令先回 ok，之後再擴充
            replyOk();
          }
        }
      } else {
        if (line.length() < 200) line += c; // 簡易防溢
      }
    }
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}

// ======= 溫控任務 =======
#if USE_FAKE_TEMP
// 一階響應假模型：讓 current 緩慢趨近 target
static void fakeTempStep(float dt) {
  float tgt, cur;
  if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
    tgt = g_targetC;
    cur = g_currentC;
    xSemaphoreGive(g_stateMutex);
  } else { tgt = g_targetC; cur = g_currentC; }

  const float tau = 8.0f;                 // 時間常數(秒)：越小越快
  const float alpha = dt / (tau + dt);
  cur = cur + alpha * (tgt - cur);

  if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
    g_currentC = cur;
    xSemaphoreGive(g_stateMutex);
  }
}
#else
// TODO: 改為你的 NTC/熱電偶讀值與 PID 控制，並寫回 g_currentC
static void realTempStep(float dt) {
  (void)dt;
  // 讀 ADC -> 換算溫度 -> g_currentC
}
#endif

static void TaskThermal(void* pv) {
  uint32_t last = millis();
  uint32_t lastPrint = millis();
#if 0
  withSerial([]{ Serial.println(F("--- Thermal task started ---")); });
#endif
  SERIAL_LOCK();
  Serial.println(F("--- Thermal task started ---"));
  SERIAL_UNLOCK();

  for (;;) {
    uint32_t now = millis();
    float dt = (now - last) / 1000.0f;
    if (dt < 0) dt = 0;
    last = now;

#if USE_FAKE_TEMP
    fakeTempStep(dt);
#else
    realTempStep(dt);
#endif

    // 每 1 秒輸出一次狀態（可關）
    if (now - lastPrint >= 1000) {
      lastPrint = now;
      float cur, tgt;
      if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        cur = g_currentC; tgt = g_targetC;
        xSemaphoreGive(g_stateMutex);
      } else { cur = g_currentC; tgt = g_targetC; }
#if 0
      withSerial([&]{
        Serial.print(F("[THERM] T:"));
        Serial.print(cur, 1);
        Serial.print(F(" / "));
        Serial.println(tgt, 1);
      });
#endif
	
		SERIAL_LOCK();
		Serial.print(F("[THERM] T:"));
		Serial.print(cur, 1);
		Serial.print(F(" / "));
		Serial.println(tgt, 1);
		SERIAL_UNLOCK();
    }

    vTaskDelay(pdMS_TO_TICKS(50));  // 20Hz 更新
  }
}

// ======= setup / loop =======
void setup() {
  // 先啟用 USB CDC
  Serial.begin(SERIAL_BAUD);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 3000)) { /* 等待最多 3 秒 */ }

  // Mutex
  g_stateMutex  = xSemaphoreCreateMutex();
  g_serialMutex = xSemaphoreCreateMutex();

  // 初值
  g_currentC = 25.0f;
  g_targetC  = DEFAULT_T_C;

  // 建立任務
  xTaskCreate(TaskGcode,   "GCODE",   512,  NULL, 2, NULL);
  xTaskCreate(TaskThermal, "THERMAL", 512,  NULL, 2, NULL);

  // 啟動排程
  vTaskStartScheduler();

  // 一般不會走到這裡
#if 0  
  withSerial([]{ Serial.println(F("Scheduler ended unexpectedly.")); });
#endif

	SERIAL_LOCK();
	Serial.println(F("Scheduler ended unexpectedly."));
	SERIAL_UNLOCK();

}

void loop() {
  // 不會執行（進入 FreeRTOS 調度）
}


#endif
//
//	EEPROM
//

#define _____宣告函式
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//														   //
//						宣告函式							   //
//														   //
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//
void IO_MODE();







/*
	函式用途: 腳位模式定義
*/
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
	pinMode(OLED_SCL , INPUT);
	pinMode(OLED_SDA , INPUT);
}


#if 0
void setup() {
  Serial.begin(115200);   // 啟動 USB CDC
  while (!Serial) { ; }   // 等待電腦連上
  Serial.println("USB CDC Serial Ready!");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();   // 讀到一個字元
    Serial.print("收到: ");
    Serial.println(c);        // 回傳顯示
  }

  delay(10);
}
#endif
