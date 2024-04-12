import {Component, OnInit} from '@angular/core';
import {NgFor,NgIf} from '@angular/common'; 

// import {MatProgressBarModule} from '@angular/material/progress-bar';

import {HttpClientModule, HttpClient, HttpParams, HttpEvent, HttpEventType} from '@angular/common/http';
import {repeat, map, retry} from 'rxjs/operators';
import {Subscription} from 'rxjs';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [
      HttpClientModule,
      NgFor, NgIf,
      // MatProgressBarModule,
  ],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent implements OnInit {
  title = 'SneakerNet';

  constructor(private http: HttpClient) { }

  catalog: SneakerNetFile[] = [
    // FIXME - test use only
    // {'filename': 'test1', 'timestamp': "none", size: 0},
    // {'filename': 'very long file name that just doesnt end test1', 'timestamp': "none", size: 0},
  ];
  ngOnInit()
  {
    this.http.get<SneakerNetFile[]>('/api/catalog')
        .pipe(
          repeat({delay:10000}), // refresh every 10 seconds
          retry({delay:1000})
        )
        .subscribe(body => this.catalog = body);
  }

  delete(catalogIndex: number)
  {
    this.http.delete(`/api/catalog/${this.catalog[catalogIndex].filename}`)
        .subscribe();
    this.catalog.splice(catalogIndex, 1);
  }

  uploadSub !: Subscription;
  uploadProgress: number = 0;
  upload(event : Event) {
    const fileSelect = event.target as HTMLInputElement;
    const file = fileSelect.files ? fileSelect.files[0] : null;
    if(file)
    {
      const timestamp = new Date(file.lastModified).toISOString();
      const upload = this.http.put(
        `/api/catalog/${file.name}`,
        file.arrayBuffer,
        { params:new HttpParams().set('timestamp', timestamp),
          reportProgress:true,
        })
        .pipe(
            map(eventObject => {
              const event = eventObject as HttpEvent<any>;
              switch(event.type) {
                  case HttpEventType.UploadProgress:
                      this.uploadProgress = event.total ? Math.round(100 * event.loaded / event.total) : 0;
                      return;
              }
            })
        );
      this.uploadSub = upload.subscribe();
    }
  }
  cancelUpload() {
    this.uploadSub?.unsubscribe();
  }
}
export interface SneakerNetFile {
  filename: string,
  timestamp: string,
  size: number,
}
