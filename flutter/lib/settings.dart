import 'package:shared_preferences/shared_preferences.dart';

// initialized by main
late final Settings settings;

class Settings {
  final SharedPreferences preferences;
  const Settings({required this.preferences});

}
