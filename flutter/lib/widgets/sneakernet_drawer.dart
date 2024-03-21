import 'package:flutter/material.dart';
import '../pages/settings.dart';
import '../pages/about.dart';

class SneakerNetDrawer extends StatelessWidget {
  @override
  Widget build(BuildContext context) => Drawer(
        child: ListView(
          padding: EdgeInsets.zero,
          children: [
            const SizedBox(
              height: 125,
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
                }),
            ListTile(
                leading: const Icon(Icons.contact_page_outlined),
                title: const Text('Contact Us'),
                onTap: () {
                  Navigator.pop(context);
                  Navigator.pushNamed(context, AboutPage.routeName);
                }),
          ],
        ),
      );
}
