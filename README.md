# pmetrics - C++ library for metrics collection

### Example

```c++
#include <pm/metrics.h>

class queue_manager_t {
public:
    queue_manager_t(pm::metrics_registry_t registry) : queue_size(registry.counter("queue_size")) {}

    void push(int i) {
       queue_size.inc();
       ...
    }

    int pop() {
       queue_size.dec();
       return ...;
    }

private:
    pm::counter_t queue_size;

    ...
}
```