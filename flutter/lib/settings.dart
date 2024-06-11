import 'package:shared_preferences/shared_preferences.dart';

class Settings {
  final SharedPreferences preferences;
  const Settings({required this.preferences});

  final String keyAutoSync = "autosync";
  bool getAutoSync(){
    // default to true
    return preferences.getBool(keyAutoSync) ?? true;
  }
  Future<void> setAutoSync(val) async {
    await preferences.setBool(keyAutoSync, val);
  }
}
