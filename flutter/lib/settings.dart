import 'package:shared_preferences/shared_preferences.dart';

class Settings {
  final SharedPreferences preferences;
  const Settings({required this.preferences});

  final String KEY_AUTO_SYNC = "autosync";
  bool getAutoSync(){
    // default to true
    return preferences.getBool(KEY_AUTO_SYNC) ?? true;
  }
  Future<void> setAutoSync(val) async {
    await preferences.setBool(KEY_AUTO_SYNC, val);
  }
}
