import 'package:flutter/material.dart';
import 'package:flutter/cupertino.dart';
import 'package:url_launcher/url_launcher.dart';

class AboutPage extends StatelessWidget {
  static const String routeName = '/about';

  const AboutPage({super.key});

  static const String email = 'sneakernet.app@gmail.com';
  static final Uri emailUri = Uri(scheme: 'mailto', path:email);

  @override
  build(BuildContext context) => Scaffold(
      appBar: AppBar(
        title: Text('SneakerNet About'),
      ),
      body: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Padding(padding: EdgeInsets.all(16)),
          Text(
            'Contact Us:',
            style: Theme.of(context).textTheme.titleLarge,
          ),
          const SizedBox(height: 8),
          ListTile(
            leading: Icon(Icons.email),
            title: Text(email),
            onTap: () async => await launchUrl(emailUri),
          ),
        ],
      ));
}
