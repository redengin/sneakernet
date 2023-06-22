import 'package:shared_preferences/shared_preferences.dart';

class Settings {
  final SharedPreferences preferences;
  const Settings({required this.preferences});

  final String KEY_DO_NOTIFY = "doNotify";
  /** default: true */
  bool getDoNotify() {
    return preferences.getBool(KEY_DO_NOTIFY) ?? true;
  }
  Future<void> setDoNotify(doNotify) async {
    await preferences.setBool(KEY_DO_NOTIFY, doNotify);
  }

  final String KEY_AUTO_SYNC = "autoSync";
  /** default: false */
  bool getAutoSync() {
    return preferences.getBool(KEY_AUTO_SYNC) ?? false;
  }
  Future<void> setAutoSync(autoSync) async {
    await preferences.setBool(KEY_AUTO_SYNC, autoSync);
  }
}
