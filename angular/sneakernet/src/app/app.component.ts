import {Component} from '@angular/core';

import {MatToolbarModule} from '@angular/material/toolbar';
import {MatIconModule} from '@angular/material/icon';
import {MatButtonModule} from '@angular/material/button';
import {MatListModule} from '@angular/material/list'; 
import {MatCardModule} from '@angular/material/card';
import {MatDialogModule} from '@angular/material/dialog';
import {MatProgressBarModule} from '@angular/material/progress-bar';
import {NgFor} from '@angular/common'; 

import {HttpClient, HttpClientModule, HttpParams} from '@angular/common/http';
import {tap} from 'rxjs/operators';
import {Observable, Subscription, finalize, lastValueFrom } from 'rxjs';
import {CommonModule} from '@angular/common';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [
      CommonModule,
      MatToolbarModule,
      MatIconModule,
      MatButtonModule,
      MatListModule,
      MatCardModule,
      MatDialogModule,
      MatProgressBarModule,
      HttpClientModule,
      NgFor,
  ],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
  title = 'SneakerNet';

  constructor(private http: HttpClient) {}

  // FIXME - test use only
  catalog = [
    {'filename': 'test1'},
    {'filename': 'test2'},
    {'filename': 'test3'},
    {'filename': 'test4'},
  ]

  getCatalog() : Observable<SneakerNetFile[]> {
    return this.http.get<SneakerNetFile[]>('/catalog', {responseType:'json'});
  }

  delete(filename: string)
  {
    this.http.delete('/catalog/' + filename);
  }

  uploadSub: Subscription | undefined;
  upload(event : Event) {
    const fileSelect = event.target as HTMLInputElement;
    const file = fileSelect.files ? fileSelect.files[0] : null;
    if(file)
    {
      const timestamp = new Date(file.lastModified).toISOString();
      const upload = this.http.put(
        `/catalog/${file.name}`,
        file.arrayBuffer,
        { params:new HttpParams().set('timestamp', timestamp),
          reportProgress:true,
        });
    }
  }
  uploadProgress: number | undefined;
  // updateProgress(message)
  // {
  // }
  cancelUpload() {
    this.uploadSub?.unsubscribe();
  }
}
export interface SneakerNetFile {
  filename: string,
  timestamp: string,
  size: number,
}
