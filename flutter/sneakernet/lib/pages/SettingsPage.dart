import 'package:flutter/material.dart';
import 'package:settings_ui/settings_ui.dart';

class SettingsPage extends StatefulWidget {
  final String title;
  const SettingsPage({super.key, required this.title});
  @override
  _SettingsState createState() => _SettingsState();
}

class _SettingsState extends State<SettingsPage> {
  bool _doNotify = true;
  bool _autoSync = false;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: SettingsList(
         sections: [
          SettingsSection(
            tiles: <SettingsTile>[
              SettingsTile.switchTile(
                leading: Icon(Icons.notification_add),
                title: Text('Notify upon entering into a SneakerNet space'),
                initialValue: _doNotify,
                onToggle: (_) { setState(() { _doNotify = _; }); },
              ),
              SettingsTile.switchTile(
                leading: Icon(Icons.sync_alt),
                title: Text('Automatically sync'),
                description: Text('Upon entering within a SneakerNet space, your '
                  'wifi will switch to SnearkerNet during the sync. \n'
                  'NOTE: internet access may be disrupted during the transfer.'),
                initialValue: _autoSync,
                onToggle: (_) { setState(() { _autoSync = _; }); },
              ),
            ],
          ),
        ],
      )
    );
  }
}