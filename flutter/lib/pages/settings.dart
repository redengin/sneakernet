import 'package:flutter/material.dart';

import 'package:settings_ui/settings_ui.dart';
import 'package:multi_select_flutter/multi_select_flutter.dart';

import '../main.dart';
import '../library.dart';

class SettingsPage extends StatefulWidget {
  static const String routeName = '/settings';

  const SettingsPage({super.key});

  @override
  createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  // final bool _autoSync = settings.getAutoSync();

  @override
  initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    // var hasLocationPermissions = Permission.locationAlways.isGranted;

    return Scaffold(
      appBar: AppBar(
        title: const Text('SneakerNet Settings'),
      ),
      body:
          SettingsList(
        sections: [
          // SettingsSection(
          //   title: Text("Permissions"),
          //   tiles: [
          //     // FutureBuilder<bool>(
          //     //   future:  _handleLocationPermissions,
          //     //   builder: (context, snapshot) =>
          //     //   SettingsTile.switchTile(
          //     //       title: Text('SneakerNet can monitor in the background'),
          //     //       initialValue: _hasLocationPermissions,
          //     //       onToggle: (_) => navigatorKey.currentState?.pushNamed(LocationPermissionsRequest.routeName),
          //     //   ),
          //     // ),
          //   ],
          // ),
          SettingsSection(
            title: const Text("SneakerNet Functionality"),
            tiles: [
              SettingsTile.switchTile(
                leading: const Icon(Icons.sync_alt),
                title: const Text('Automatically sync'),
                description: const Text(
                  'WiFi connection will be temporarily interrupted '
                  'to sync with SnearkerNet'),
                initialValue: settings.getAutoSync(),
                onToggle: (_) => setState(() {
                  settings.setAutoSync(_);
                }),
              ),
            ],
          ),
          SettingsSection(
            title: const Text("Moderate SneakerNet Content"),
            tiles: [
              SettingsTile.navigation(
                leading: const Icon(Icons.delete_outline),
                title: const Text('Manage unwanted files'),
                description: const Text("deleted files will not be gathered "
                    "from other nodes"),
                onPressed: (_) => showDialog(
                    context: context,
                    builder: (context) {
                      return LibraryFilterPicker(
                          library.unwantedFiles, "Unwanted");
                    }),
              ),
              SettingsTile.navigation(
                leading: const Icon(Icons.flag_outlined),
                title: const Text('Manage flagged files'),
                description: const Text("flagged files will be removed from "
                    "other nodes"),
                onPressed: (_) => showDialog(
                    context: context,
                    builder: (context) {
                      return LibraryFilterPicker(
                          library.flaggedFiles, "Flagged");
                    }),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class LibraryFilterPicker extends StatelessWidget {
  final StringListFile filterSettings;
  final String message;

  const LibraryFilterPicker(this.filterSettings, this.message, {super.key});

  @override
  Widget build(BuildContext context) {
    final items = filterSettings
        .list()
        .map((e) => MultiSelectItem<String>(e, e))
        .toList(growable: false);
    return MultiSelectDialog(
      backgroundColor: Colors.deepOrangeAccent,
      title: Text('Remove Filter for $message files'),
      items: items,
      initialValue: const <String>[],
      onConfirm: (values) => filterSettings.remove(values),
    );
  }
}
