# 🚀 Quick Reference - Phone Updates

## 📝 Edit Code (Phone)
1. Open GitHub app or github.com
2. Edit `src/main.cpp`
3. Commit changes
4. Wait 2-3 minutes for auto-build

## 📥 Download Firmware
1. Go to repository → "Actions" tab
2. Click latest successful build
3. Download artifact (zip file)
4. Extract the `.bin` file

## 📤 Upload to Cube
1. Turn on cube, wait 10 seconds
2. Connect phone to WiFi: **`ShakeCube-Config`** (password: `shakecube123`)
3. Open browser → **`http://192.168.4.1/update`**
4. Choose the `.bin` file
5. Click "Update"
6. Wait 30 seconds - Done! ✅

---

## 📌 Key Info
- **WiFi Name:** ShakeCube-Config
- **WiFi Password:** shakecube123
- **Update URL:** http://192.168.4.1/update
- **Cube IP:** 192.168.4.1

---

## 🎯 Common Edits

### Change Animation Speed (line ~300)
```cpp
unsigned long duration = random(5000, 10001); // 5-10 seconds
```

### Change Sensitivity (line ~417)
```cpp
if (millis() - lastShakeTime > 1000) { // milliseconds
```

### Change Brightness (line ~336)
```cpp
lc.setIntensity(0, 8); // 0-15 (higher = brighter)
```

---

**Full guide:** See `PHONE_UPDATE_GUIDE.md`

