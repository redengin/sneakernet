import 'package:flutter/material.dart';
import '../pages/settings.dart';

class SneakerNetDrawer extends StatelessWidget {
  @override
  Widget build(BuildContext context) => Drawer(
      child: ListView(
        padding: EdgeInsets.zero,
        children: [
          const SizedBox(
            height: 100,
            child: DrawerHeader(
              decoration: BoxDecoration(color: Colors.blue),
              child: Text('SneakerNet',
                  style: TextStyle(fontSize: 30, color: Colors.white)),
            ),
          ),
          ListTile(
              leading: const Icon(Icons.settings),
              title: const Text('Settings'),
              onTap: () {
                Navigator.pop(context);
                Navigator.pushNamed(context, SettingsPage.routeName);
              })
        ],
      ),
    );
}