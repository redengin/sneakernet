import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:settings_ui/settings_ui.dart';
import 'package:multi_select_flutter/multi_select_flutter.dart';
import '../settings.dart';
import '../library.dart';

class SettingsPage extends StatefulWidget {
  static const String routeName = '/settings';

  const SettingsPage({super.key});

  @override
  createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  late bool _doNotify;
  late bool _autoSync;

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: const Text('SneakerNet Settings'),
        ),
        body: SettingsList(
          sections: [
            SettingsSection(
              tiles: [
                // Disabled, doesn't appear you can change wifi connection from background
                // SettingsTile.switchTile(
                //   leading: Icon(Icons.sync_alt),
                //   title: Text('Automatically sync'),
                //   description: Text(
                //     'Upon entering within a SneakerNet space, your '
                //     'wifi connection will be temporarily interrupted '
                //     'to sync with SnearkerNet'),
                //   initialValue: settings.getAutoSync(),
                //   onToggle: (_) => setState(() {
                //     settings.setAutoSync(_);
                //   }),
                // ),
                SettingsTile.navigation(
                  leading: const Icon(Icons.delete_outline),
                  title: const Text('Manage unwanted files'),
                  description: const Text("deleted files will not be gathered "
                    "from other nodes"),
                  onPressed: (_) => showDialog(
                      context: context,
                      builder: (context) {
                        return LibraryFilterPicker(library.unwantedFiles, "Unwanted");
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
                        return LibraryFilterPicker(library.flaggedFiles, "Flagged");
                      }),
                ),
              ],
            ),
          ],
        ));
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
      onConfirm: (values) => filterSettings.removeEach(values),
    );
  }
}
