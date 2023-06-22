import 'package:flutter/material.dart';
import 'package:settings_ui/settings_ui.dart';
import 'package:sneakernet/settings.dart';

class SettingsPage extends StatefulWidget {
  static const String routeName = '/settings';

  final Settings settings;
  const SettingsPage({super.key, required this.settings});

  @override
  createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  late bool _doNotify;
  late bool _autoSync;

  @override void initState() {
    // TODO: implement initState
    super.initState();
    _doNotify = widget.settings.getDoNotify();
    _autoSync = widget.settings.getAutoSync();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text('SneakerNet Settings'),
        ),
        body: SettingsList(
          sections: [
            SettingsSection(
              tiles: <SettingsTile>[
                SettingsTile.switchTile(
                  leading: Icon(Icons.notification_add),
                  title: Text('Notify upon entering into a SneakerNet space'),
                  initialValue: _doNotify,
                  onToggle: (_) => setState(() {
                    _doNotify = _;
                    widget.settings.setDoNotify(_);
                  }),
                ),
                SettingsTile.switchTile(
                  leading: Icon(Icons.sync_alt),
                  title: Text('Automatically sync'),
                  description: Text(
                      'Upon entering within a SneakerNet space, your '
                      'wifi will switch to SnearkerNet during the sync. \n'
                      'NOTE: internet access may be disrupted during the transfer.'),
                  initialValue: _autoSync,
                  onToggle: (_) => setState(() {
                    _autoSync = _;
                    widget.settings.setAutoSync(_);
                  }),
                ),
              ],
            ),
          ],
        ));
  }
}

// FIXME
// if (await Permission.locationWhenInUse.request().isGranted) {
//   await Permission.locationAlways.request().isGranted;
// }
