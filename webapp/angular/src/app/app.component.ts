import { Component } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { retry } from 'rxjs';

import { NgIcon } from '@ng-icons/core';

import { Toolbar } from './components/toolbar';

// Types per openapi/catalog.yml
//==============================================================================
type Folder = {
  locked?: boolean;
  subfolders?: string[];
  files?: File[];
};
type File = {
  name?: string;
  size?: BigInteger;
  timestamp?: string;
  title?: string;
  hasIcon?: boolean;
}

@Component({
  selector: 'app-root',
  imports: [ Toolbar, NgIcon ],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
  constructor(private http: HttpClient) { }
  ngOnInit() { this.getPathData(); }

  currentPath = "/";
  // currentPath = "/short";
  folderData: Folder = {};

  getPathData() : void
  {
    this.http.get<Folder>(`api/catalog${this.currentPath}`)
      .pipe( retry({ delay: 500 /* ms */ }))
      .subscribe(body => this.folderData = body);
  }
}

