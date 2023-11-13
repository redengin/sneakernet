import 'package:flutter/material.dart';
import 'package:permission_handler/permission_handler.dart';

class LocationPermissionsRequest extends StatelessWidget {
  static String routeName = '/requestLocationPermissions';

  const LocationPermissionsRequest({super.key});

  @override
  Widget build(BuildContext context) => AlertDialog(
        title: const Text("Location Permissions"),
        content: const SingleChildScrollView(
            child: ListBody(
          children: <Widget>[
            Text(
                "Sneakernet needs your permission to sync in the background (it may ask twice)"),
            Text("    1. Grant 'WHILE USING THE APP'"),
            Text("    2. Grant 'Allow all the time'"),
          ],
        )),
        actions: [
          TextButton(
            child: const Text("Deny"),
            onPressed: () => Navigator.pop(context),
          ),
          TextButton(
            child: const Text("Grant"),
            onPressed: () async {
              await requestPermissions(context);
              if(!context.mounted) return;
              Navigator.of(context).pop();
            },
          ),
        ],
      );

  requestPermissions(BuildContext context) async {
    // must ask twice (WhenInUse before asking for Always)
    await Permission.locationWhenInUse.request();
    if (await Permission.locationWhenInUse.isGranted) {
      await Permission.locationAlways.request();
    }
  }
}
