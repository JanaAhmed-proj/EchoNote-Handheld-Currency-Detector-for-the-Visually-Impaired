#include <SPI.h>
#include <egp5__inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"
#include <HardwareSerial.h>

// ==========================================
// 1. LCD CONFIGURATION & PINS
// ==========================================
#define SCK  14
#define MOSI 13
#define DC   2
#define RST  12

#define RED      0xF800
#define GREEN    0x07E0
#define BLUE     0x001F
#define WHITE    0xFFFF
#define BLACK    0x0000
#define YELLOW   0xFFE0
#define ORANGE   0xFD20
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define DARKGRAY 0x4208

// 5x7 font (uppercase + numbers only)
static const uint8_t font[] = {
  0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x5F,0x00,0x00, 0x00,0x07,0x00,0x07,0x00,
  0x14,0x7F,0x14,0x7F,0x14, 0x24,0x2A,0x7F,0x2A,0x12, 0x23,0x13,0x08,0x64,0x62,
  0x36,0x49,0x55,0x22,0x50, 0x00,0x05,0x03,0x00,0x00, 0x00,0x1C,0x22,0x41,0x00,
  0x00,0x41,0x22,0x1C,0x00, 0x14,0x08,0x3E,0x08,0x14, 0x08,0x08,0x3E,0x08,0x08,
  0x00,0x50,0x30,0x00,0x00, 0x08,0x08,0x08,0x08,0x08, 0x00,0x60,0x60,0x00,0x00,
  0x20,0x10,0x08,0x04,0x02, 0x3E,0x51,0x49,0x45,0x3E, 0x00,0x42,0x7F,0x40,0x00,
  0x42,0x61,0x51,0x49,0x46, 0x21,0x41,0x45,0x4B,0x31, 0x18,0x14,0x12,0x7F,0x10,
  0x27,0x45,0x45,0x45,0x39, 0x3C,0x4A,0x49,0x49,0x30, 0x01,0x71,0x09,0x05,0x03,
  0x36,0x49,0x49,0x49,0x36, 0x06,0x49,0x49,0x29,0x1E, 0x00,0x36,0x36,0x00,0x00,
  0x00,0x56,0x36,0x00,0x00, 0x08,0x14,0x22,0x41,0x00, 0x14,0x14,0x14,0x14,0x14,
  0x00,0x41,0x22,0x14,0x08, 0x02,0x01,0x51,0x09,0x06, 0x32,0x49,0x79,0x41,0x3E,
  0x7E,0x11,0x11,0x11,0x7E, 0x7F,0x49,0x49,0x49,0x36, 0x3E,0x41,0x41,0x41,0x22,
  0x7F,0x41,0x41,0x22,0x1C, 0x7F,0x49,0x49,0x49,0x41, 0x7F,0x09,0x09,0x09,0x01,
  0x3E,0x41,0x49,0x49,0x7A, 0x7F,0x08,0x08,0x08,0x7F, 0x00,0x41,0x7F,0x41,0x00,
  0x20,0x40,0x41,0x3F,0x01, 0x7F,0x08,0x14,0x22,0x41, 0x7F,0x40,0x40,0x40,0x40,
  0x7F,0x02,0x0C,0x02,0x7F, 0x7F,0x04,0x08,0x10,0x7F, 0x3E,0x41,0x41,0x41,0x3E,
  0x7F,0x09,0x09,0x09,0x06, 0x3E,0x41,0x51,0x21,0x5E, 0x7F,0x09,0x19,0x29,0x46,
  0x46,0x49,0x49,0x49,0x31, 0x01,0x01,0x7F,0x01,0x01, 0x3F,0x40,0x40,0x40,0x3F,
  0x1F,0x20,0x40,0x20,0x1F, 0x3F,0x40,0x38,0x40,0x3F, 0x63,0x14,0x08,0x14,0x63,
  0x07,0x08,0x70,0x08,0x07, 0x61,0x51,0x49,0x45,0x43
};

void lcdCmd(uint8_t c)  { digitalWrite(DC, LOW);  SPI.transfer(c); }
void lcdData(uint8_t d) { digitalWrite(DC, HIGH); SPI.transfer(d); }

void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  lcdCmd(0x2A); lcdData(x0>>8); lcdData(x0); lcdData(x1>>8); lcdData(x1);
  lcdCmd(0x2B); lcdData(y0>>8); lcdData(y0); lcdData(y1>>8); lcdData(y1);
  lcdCmd(0x2C);
}

void fillScreen(uint16_t color) {
  setWindow(0, 0, 239, 239);
  uint8_t hi = color >> 8, lo = color & 0xFF;
  uint8_t rowBuf[480];
  for (int i = 0; i < 240; i++) { rowBuf[i*2] = hi; rowBuf[i*2+1] = lo; }
  digitalWrite(DC, HIGH);
  for (int y = 0; y < 240; y++) SPI.writeBytes(rowBuf, 480);
}

void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  setWindow(x, y, x+w-1, y+h-1);
  uint8_t hi = color >> 8, lo = color & 0xFF;
  uint8_t rowBuf[w*2];
  for (int i = 0; i < w; i++) { rowBuf[i*2] = hi; rowBuf[i*2+1] = lo; }
  digitalWrite(DC, HIGH);
  for (int i = 0; i < h; i++) SPI.writeBytes(rowBuf, w*2);
}

void drawHLine(int16_t x, int16_t y, int16_t len, uint16_t color) {
  if (y < 0 || y > 239 || x > 239) return;
  if (x < 0) { len += x; x = 0; }
  if (x + len > 240) len = 240 - x;
  if (len <= 0) return;
  setWindow(x, y, x+len-1, y);
  uint8_t hi = color>>8, lo = color&0xFF;
  digitalWrite(DC, HIGH);
  for (int i = 0; i < len; i++) { SPI.transfer(hi); SPI.transfer(lo); }
}

void drawVLine(int16_t x, int16_t y, int16_t len, uint16_t color) {
  if (x < 0 || x > 239 || y > 239) return;
  if (y < 0) { len += y; y = 0; }
  if (y + len > 240) len = 240 - y;
  if (len <= 0) return;
  uint8_t hi = color>>8, lo = color&0xFF;
  for (int i = 0; i < len; i++) {
    setWindow(x, y+i, x, y+i);
    digitalWrite(DC, HIGH);
    SPI.transfer(hi); SPI.transfer(lo);
  }
}

void drawBBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  for (int t = 0; t < 2; t++) {
    drawHLine(x+t, y+t, w-2*t, color);
    drawHLine(x+t, y+h-1-t, w-2*t, color);
    drawVLine(x+t, y+t, h-2*t, color);
    drawVLine(x+w-1-t, y+t, h-2*t, color);
  }
}

void drawChar(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg, uint8_t size) {
  if (c < 32 || c > 90) return;
  const uint8_t* glyph = font + (c - 32) * 5;
  uint16_t w = 5 * size, h = 7 * size;
  setWindow(x, y, x+w-1, y+h-1);
  uint8_t fgHi=fg>>8, fgLo=fg&0xFF, bgHi=bg>>8, bgLo=bg&0xFF;
  uint8_t rowBuf[300];
  digitalWrite(DC, HIGH);
  for (int row = 0; row < 7; row++) {
    for (int r = 0; r < size; r++) {
      int ptr = 0;
      for (int col = 0; col < 5; col++) {
        bool on = (glyph[col] & (1 << row));
        for (int cs = 0; cs < size; cs++) {
          rowBuf[ptr++] = on ? fgHi : bgHi;
          rowBuf[ptr++] = on ? fgLo : bgLo;
        }
      }
      SPI.writeBytes(rowBuf, w*2);
    }
  }
}

void drawString(int16_t x, int16_t y, const char* str, uint16_t fg, uint16_t bg, uint8_t size) {
  while (*str) {
    char c = *str++;
    if (c >= 'a' && c <= 'z') c -= 32;
    drawChar(x, y, c, fg, bg, size);
    x += 6 * size;
  }
}

void drawConfBar(int16_t x, int16_t y, int16_t w, int16_t h, float pct, uint16_t fg, uint16_t bg) {
  fillRect(x, y, w, h, bg);
  int filled = (int)(pct * w);
  if (filled > 0) fillRect(x, y, filled, h, fg);
}


// ==========================================
// 2. DFPLAYER MINI (MP3-TF-16P) DRIVER
// ------------------------------------------
// WIRING:
//   DFPlayer RX  ←  ESP32 pin 15  (via 1kΩ resistor recommended)
//   DFPlayer TX  →  ESP32 pin 4
//   DFPlayer VCC → 5V
//   DFPlayer GND → GND
//   Speaker       → SPK_1 / SPK_2
//
// SD CARD FILE NAMES (must be 4-digit zero-padded in /mp3/ folder):
//   /mp3/0050.mp3  ← played when label "50" detected
//   /mp3/0010.mp3  ← played when label "10" detected
//   /mp3/0005.mp3  ← played when label "5"  detected
//
// Alternatively place them in root as 0050.mp3 etc. if not using /mp3/ folder.
// ==========================================
#define DFPLAYER_RX_PIN  4   // ESP32 GPIO4  ← DFPlayer TX
#define DFPLAYER_TX_PIN  15  // ESP32 GPIO15 → DFPlayer RX
#define DFPLAYER_VOLUME  25  // Volume 0–30

HardwareSerial dfSerial(2); // Use UART2

// Send a raw 10-byte DFPlayer command frame (no ACK mode — non-blocking)
void dfSendCmd(uint8_t cmd, uint8_t paramHi, uint8_t paramLo) {
  uint8_t buf[10];
  buf[0] = 0x7E;
  buf[1] = 0xFF;
  buf[2] = 0x06;
  buf[3] = cmd;
  buf[4] = 0x00; // no ACK feedback
  buf[5] = paramHi;
  buf[6] = paramLo;
  int16_t checksum = -(buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6]);
  buf[7] = checksum >> 8;
  buf[8] = checksum & 0xFF;
  buf[9] = 0xEF;
  dfSerial.write(buf, 10);
  delay(30); // give module time to process command
}

void dfReset()              { dfSendCmd(0x0C, 0x00, 0x00); }
void dfSetVolume(uint8_t v) { dfSendCmd(0x06, 0x00, v);    }
void dfStop()               { dfSendCmd(0x16, 0x00, 0x00); }

// Play a file by its number (plays /mp3/XXXX.mp3 on SD card)
void dfPlayFile(uint16_t fileNum) {
  dfSendCmd(0x03, fileNum >> 8, fileNum & 0xFF);
}

// ==========================================
// MS_PER_PLAY: how long to wait after triggering
// playback before sending the next command.
// Set this to your actual MP3 duration + ~300ms margin.
// e.g. if your files are ~2 seconds long, set to 2300.
// ==========================================
#define MS_PER_PLAY 3000

void dfPlayAndWait(uint16_t fileNum) {
  dfPlayFile(fileNum);
  delay(MS_PER_PLAY);
}

// Map label string to SD card file number.
// "50" → 50 (plays 0050.mp3)
// "10" → 10 (plays 0010.mp3)
// "5"  →  5 (plays 0005.mp3)
// Returns 0 for unknown labels (no sound).
uint16_t labelToFileNumber(const char* label) {
  if (strcmp(label, "50") == 0) return 50;
  if (strcmp(label, "10") == 0) return 10;
  if (strcmp(label, "5")  == 0) return 5;
  return 0;
}

// Play the sound for a label TWICE with a gap between plays
void playLabelSound(const char* label) {
  uint16_t fileNum = labelToFileNumber(label);
  if (fileNum == 0) {
    Serial.printf("No sound mapped for label: %s\n", label);
    return;
  }
  Serial.printf("Playing /mp3/%04d.mp3 x2 for label '%s'\n", fileNum, label);
  dfPlayAndWait(fileNum); // first play
  delay(400);             // gap between repetitions
  dfPlayAndWait(fileNum); // second play
}


// ==========================================
// 3. CAMERA CONFIGURATION & AI VARIABLES
// ==========================================
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM   0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM     5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

uint8_t *snapshot_buf;

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
  size_t pixel_ix = offset * 3;
  size_t out_ptr_ix = 0;
  while (length--) {
    // Pack as 0x00RRGGBB (Edge Impulse standard, R at index 0)
    out_ptr[out_ptr_ix++] = (snapshot_buf[pixel_ix]   << 16)
                          | (snapshot_buf[pixel_ix+1] <<  8)
                          |  snapshot_buf[pixel_ix+2];
    pixel_ix += 3;
  }
  return 0;
}

#define VOTE_FRAMES     3
#define VOTE_THRESHOLD  2
#define CONF_HIGH       0.70f
#define CONF_LOW        0.45f

struct DetectionResult {
  char  label[32];
  float confidence;
  int   votes;
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
  uint32_t x, y, w, h;
#endif
};

#define MAX_RESULTS 8
DetectionResult detections[MAX_RESULTS];
int detectionCount = 0;


// ==========================================
// 4. SETUP
// ==========================================
void setup() {
  delay(3000);
  Serial.begin(115200);

  // --- LCD init ---
  pinMode(DC, OUTPUT); pinMode(RST, OUTPUT);
  digitalWrite(RST, LOW); delay(50); digitalWrite(RST, HIGH); delay(150);
  SPI.begin(SCK, -1, MOSI, -1);
  SPI.setFrequency(40000000);
  lcdCmd(0x11); delay(120);
  lcdCmd(0x3A); lcdData(0x55);
  lcdCmd(0x36); lcdData(0x00);
  lcdCmd(0x21); lcdCmd(0x29); delay(50);

  fillScreen(BLACK);
  drawString(40, 100, "HELLO", WHITE, BLACK, 4);
  delay(800);

  // --- DFPlayer init ---
  fillScreen(BLACK);
  drawString(10, 100, "INIT AUDIO", WHITE, BLACK, 2);
  dfSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
  delay(1000);    // DFPlayer needs ~1s after power-on before accepting commands
  dfReset();
  delay(1500);    // wait for reset to complete
  dfSetVolume(DFPLAYER_VOLUME);
  delay(200);
  Serial.println("DFPlayer ready");

  // --- Allocate AI buffer (PSRAM) ---
  fillScreen(BLACK);
  drawString(10, 100, "INIT CAMERA", WHITE, BLACK, 2);
  snapshot_buf = (uint8_t*)ps_malloc(240 * 240 * 3);
  if (!snapshot_buf) {
    fillScreen(RED); drawString(10, 100, "MEM ERR", WHITE, RED, 3);
    while (true);
  }

  // --- Camera init ---
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM; config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM; config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM; config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM; config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size   = FRAMESIZE_240X240;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  config.jpeg_quality = 12;

  if (psramFound()) {
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.fb_count    = 2;
  } else {
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count    = 1;
  }

  if (esp_camera_init(&config) != ESP_OK) {
    fillScreen(RED); drawString(10, 100, "CAM FAIL", WHITE, RED, 3);
    while (true);
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
  s->set_brightness(s, 1);
  s->set_saturation(s, 2);

  // Camera warm-up: flush frames so AEC/AWB stabilises
  fillScreen(BLACK);
  drawString(10, 110, "WARMING UP", WHITE, BLACK, 2);
  for (int i = 0; i < 15; i++) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) esp_camera_fb_return(fb);
    delay(80);
  }

  fillScreen(BLACK);
  drawString(30, 110, "READY", GREEN, BLACK, 3);
  delay(600);
}


// ==========================================
// 5. HELPERS
// ==========================================
void flushFrames(int count, int delayMs) {
  for (int i = 0; i < count; i++) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) esp_camera_fb_return(fb);
    delay(delayMs);
  }
}

// Capture one frame into snapshot_buf as RGB888 (R=[0], G=[1], B=[2])
bool captureToBuffer() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return false;
  for (int i = 0; i < 240 * 240; i++) {
    uint8_t hi = fb->buf[i * 2];
    uint8_t lo = fb->buf[i * 2 + 1];
    snapshot_buf[i * 3 + 0] = (hi & 0xF8);                               // R
    snapshot_buf[i * 3 + 1] = ((hi & 0x07) << 5) | ((lo & 0xE0) >> 3);  // G
    snapshot_buf[i * 3 + 2] = (lo & 0x1F) << 3;                          // B
  }
  esp_camera_fb_return(fb);
  return true;
}

// Re-encode snapshot_buf back to RGB565 and push to LCD
void displaySnapshot() {
  setWindow(0, 0, 239, 239);
  uint8_t rowBuf[480];
  digitalWrite(DC, HIGH);
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 240; x++) {
      uint8_t r = snapshot_buf[(y*240+x)*3+0];
      uint8_t g = snapshot_buf[(y*240+x)*3+1];
      uint8_t b = snapshot_buf[(y*240+x)*3+2];
      uint16_t px = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
      rowBuf[x*2]   = px >> 8;
      rowBuf[x*2+1] = px & 0xFF;
    }
    SPI.writeBytes(rowBuf, 480);
  }
}


// ==========================================
// 6. VOTED INFERENCE
// ==========================================
int runVotedInference() {
  detectionCount = 0;

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
  struct ObjVote {
    char  label[32];
    float confSum;
    int   votes;
    uint32_t x, y, w, h;
    float bestConf;
  } objVotes[MAX_RESULTS];
  int objVoteCount = 0;

  for (int attempt = 0; attempt < VOTE_FRAMES; attempt++) {
    if (attempt > 0) flushFrames(2, 60);
    if (!captureToBuffer()) continue;

    if (EI_CLASSIFIER_INPUT_WIDTH != 240 || EI_CLASSIFIER_INPUT_HEIGHT != 240) {
      ei::image::processing::crop_and_interpolate_rgb888(
        snapshot_buf, 240, 240,
        snapshot_buf, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    ei_impulse_result_t result = {0};
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
    if (err != EI_IMPULSE_OK) { Serial.printf("Classifier error: %d\n", err); continue; }

    for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
      if (result.bounding_boxes[i].value < CONF_LOW) continue;
      const char* lbl = result.bounding_boxes[i].label;
      float conf = result.bounding_boxes[i].value;
      int slot = -1;
      for (int v = 0; v < objVoteCount; v++)
        if (strcmp(objVotes[v].label, lbl) == 0) { slot = v; break; }
      if (slot == -1 && objVoteCount < MAX_RESULTS) {
        slot = objVoteCount++;
        strncpy(objVotes[slot].label, lbl, 31);
        objVotes[slot].confSum = 0; objVotes[slot].votes = 0; objVotes[slot].bestConf = 0;
      }
      if (slot >= 0) {
        objVotes[slot].confSum += conf;
        objVotes[slot].votes++;
        if (conf > objVotes[slot].bestConf) {
          objVotes[slot].bestConf = conf;
          objVotes[slot].x = result.bounding_boxes[i].x;
          objVotes[slot].y = result.bounding_boxes[i].y;
          objVotes[slot].w = result.bounding_boxes[i].width;
          objVotes[slot].h = result.bounding_boxes[i].height;
        }
      }
    }
  }

  for (int v = 0; v < objVoteCount; v++) {
    if (objVotes[v].votes >= VOTE_THRESHOLD && detectionCount < MAX_RESULTS) {
      DetectionResult &d = detections[detectionCount++];
      strncpy(d.label, objVotes[v].label, 31);
      d.confidence = objVotes[v].confSum / objVotes[v].votes;
      d.votes      = objVotes[v].votes;
      d.x = objVotes[v].x; d.y = objVotes[v].y;
      d.w = objVotes[v].w; d.h = objVotes[v].h;
    }
  }

#else
  float confAccum[EI_CLASSIFIER_LABEL_COUNT] = {0};
  int   voteCount[EI_CLASSIFIER_LABEL_COUNT] = {0};

  for (int attempt = 0; attempt < VOTE_FRAMES; attempt++) {
    if (attempt > 0) flushFrames(2, 60);
    if (!captureToBuffer()) continue;

    if (EI_CLASSIFIER_INPUT_WIDTH != 240 || EI_CLASSIFIER_INPUT_HEIGHT != 240) {
      ei::image::processing::crop_and_interpolate_rgb888(
        snapshot_buf, 240, 240,
        snapshot_buf, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    ei_impulse_result_t result = {0};
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
    if (err != EI_IMPULSE_OK) { Serial.printf("Classifier error: %d\n", err); continue; }

    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
      float c = result.classification[i].value;
      if (c >= CONF_LOW) { confAccum[i] += c; voteCount[i]++; }
    }
  }

  for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    if (voteCount[i] >= VOTE_THRESHOLD && detectionCount < MAX_RESULTS) {
      DetectionResult &d = detections[detectionCount++];
      strncpy(d.label, ei_classifier_inferencing_categories[i], 31);
      d.confidence = confAccum[i] / voteCount[i];
      d.votes      = voteCount[i];
    }
  }

  // Sort descending by confidence
  for (int i = 0; i < detectionCount-1; i++)
    for (int j = i+1; j < detectionCount; j++)
      if (detections[j].confidence > detections[i].confidence) {
        DetectionResult tmp = detections[i]; detections[i] = detections[j]; detections[j] = tmp;
      }
#endif

  return detectionCount;
}


// ==========================================
// 7. DISPLAY RESULTS
// ==========================================
void displayResults(unsigned long inferenceMs) {
  fillRect(0, 0, 240, 18, BLUE);
  drawString(4, 4, "RESULTS", WHITE, BLUE, 1);
  char timeStr[16];
  sprintf(timeStr, "%lums", inferenceMs);
  drawString(240 - (strlen(timeStr)*6) - 2, 4, timeStr, CYAN, BLUE, 1);

  int cursorY = 22;
  bool anyHigh = false;

  if (detectionCount == 0) {
    fillRect(0, 18, 240, 222, BLACK);
    drawString(20, 100, "NO OBJECTS", DARKGRAY, BLACK, 2);
    drawString(8, 120, "DETECTED", DARKGRAY, BLACK, 2);
    return;
  }

  fillRect(0, 18, 240, 222, BLACK);

  for (int i = 0; i < detectionCount && cursorY < 210; i++) {
    DetectionResult &d = detections[i];
    bool isHigh = (d.confidence >= CONF_HIGH);
    uint16_t labelColor = isHigh ? GREEN : YELLOW;
    uint16_t barColor   = isHigh ? GREEN : ORANGE;
    if (isHigh) anyHigh = true;

    char upperLabel[32];
    strncpy(upperLabel, d.label, 31);
    for (int c = 0; upperLabel[c]; c++)
      if (upperLabel[c] >= 'a' && upperLabel[c] <= 'z') upperLabel[c] -= 32;

    drawString(4, cursorY, upperLabel, labelColor, BLACK, 2);
    if (!isHigh) {
      int labelW = strlen(upperLabel) * 12;
      drawString(labelW + 6, cursorY + 2, "LOW", ORANGE, BLACK, 1);
    }
    cursorY += 16;

    char pctStr[8];
    sprintf(pctStr, "%d%%", (int)(d.confidence * 100));
    drawString(4, cursorY, pctStr, WHITE, BLACK, 1);
    drawConfBar(26, cursorY, 170, 8, d.confidence, barColor, DARKGRAY);

    for (int v = 0; v < VOTE_FRAMES; v++) {
      uint16_t dotColor = (v < d.votes) ? GREEN : DARKGRAY;
      fillRect(200 + v*8, cursorY, 5, 8, dotColor);
    }
    cursorY += 14;

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    float scaleX = 240.0f / EI_CLASSIFIER_INPUT_WIDTH;
    float scaleY = 240.0f / EI_CLASSIFIER_INPUT_HEIGHT;
    drawBBox((int16_t)(d.x*scaleX), (int16_t)(d.y*scaleY),
             (int16_t)(d.w*scaleX), (int16_t)(d.h*scaleY), labelColor);
#endif
  }

  uint16_t footerBg = anyHigh ? GREEN : ORANGE;
  fillRect(0, 220, 240, 20, footerBg);
  if (anyHigh) drawString(60, 222, "CONFIDENT",      BLACK, footerBg, 1);
  else         drawString(40, 222, "LOW CONFIDENCE", BLACK, footerBg, 1);
}


// ==========================================
// 8. PLAY AUDIO FOR TOP CONFIDENT DETECTION
// Plays the sound TWICE. Only fires if the
// top result is ≥ CONF_HIGH and is one of
// the known labels: 50, 10, 5.
// ==========================================
void playDetectionAudio() {
  for (int i = 0; i < detectionCount; i++) {
    if (detections[i].confidence < CONF_HIGH) continue;
    if (labelToFileNumber(detections[i].label) == 0) continue;

    // Show "PLAYING <label>" in the footer while audio plays
    fillRect(0, 220, 240, 20, CYAN);
    char footerStr[24];
    sprintf(footerStr, "PLAYING %s", detections[i].label);
    drawString(4, 222, footerStr, BLACK, CYAN, 1);

    // Play the sound TWICE
    playLabelSound(detections[i].label);

    // Restore footer
    fillRect(0, 220, 240, 20, GREEN);
    drawString(60, 222, "CONFIDENT", BLACK, GREEN, 1);
    return; // only play the single highest-confidence result
  }
  // If nothing qualifies, do nothing (no sound for low-conf or unknown labels)
}


// ==========================================
// 9. MAIN LOOP
// ==========================================
void loop() {

  // === STEP 1: LIVE VIEWFINDER (5 seconds) ===
  unsigned long start = millis();
  while (millis() - start < 5000) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) {
      setWindow(0, 0, 239, 239);
      digitalWrite(DC, HIGH);
      uint8_t *ptr = fb->buf;
      for (int y = 0; y < 240; y++) { SPI.writeBytes(ptr, 480); ptr += 480; }
      esp_camera_fb_return(fb);
    }
  }

  // === STEP 2: COUNTDOWN 3, 2, 1 ===
  for (int i = 3; i > 0; i--) {
    fillScreen(BLACK);
    char num[2]; sprintf(num, "%d", i);
    drawString(90, 70, num, WHITE, BLACK, 15);
    flushFrames(4, 80); // flush during countdown so AEC settles
    delay(600);
  }

  // === STEP 3: CAPTURE FROZEN FRAME ===
  fillScreen(BLACK);
  drawString(10, 110, "CAPTURING", GREEN, BLACK, 2);
  flushFrames(3, 50); // final flush right before the shot

  if (!captureToBuffer()) {
    fillScreen(RED);
    drawString(10, 100, "CAM ERROR", WHITE, RED, 3);
    delay(3000);
    return;
  }

  displaySnapshot();
  drawString(4, 224, "ANALYZING", CYAN, BLACK, 1);
  delay(500);

  // === STEP 4: VOTED INFERENCE ===
  unsigned long t0 = millis();
  int found = runVotedInference();
  unsigned long inferenceMs = millis() - t0;

  Serial.printf("Inference: %lums | %d detection(s)\n", inferenceMs, found);
  for (int i = 0; i < found; i++)
    Serial.printf("  [%d] label=%s  conf=%.2f  votes=%d/%d\n",
      i, detections[i].label, detections[i].confidence,
      detections[i].votes, VOTE_FRAMES);

  // === STEP 5: SHOW RESULTS ===
  displaySnapshot();
  displayResults(inferenceMs);

  // === STEP 6: PLAY AUDIO TWICE for top detection ===
  // Audio plays while the result screen stays visible.
  // Total audio time = 2 × MS_PER_PLAY + 400ms gap.
  playDetectionAudio();

  // Hold results for remaining time after audio finishes
  delay(1500);
}
