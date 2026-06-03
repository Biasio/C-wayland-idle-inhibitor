#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wayland-client.h>
#include "idle-inhibit-unstable-v1-client-protocol.h"

static struct wl_compositor *compositor;
static struct zwp_idle_inhibit_manager_v1 *idle_inhibit_manager;

static void handle_signal(int sig) {
    _exit(0);
}

static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface, uint32_t version) {
    if (strcmp(interface, wl_compositor_interface.name) == 0)
        compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    else if (strcmp(interface, zwp_idle_inhibit_manager_v1_interface.name) == 0)
        idle_inhibit_manager = wl_registry_bind(registry, name, &zwp_idle_inhibit_manager_v1_interface, 1);
}

static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return 1;
    }

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    if (!compositor || !idle_inhibit_manager) {
        fprintf(stderr, "Wayland seems not to support idle_inhibit_unstable_v1 protocol.\n");
        wl_registry_destroy(registry);
        wl_display_disconnect(display);
        return 1;
    }

    wl_registry_destroy(registry);
    struct wl_surface *surface = wl_compositor_create_surface(compositor);
    struct zwp_idle_inhibitor_v1 *inhibitor =
        zwp_idle_inhibit_manager_v1_create_inhibitor(idle_inhibit_manager, surface);
    wl_display_roundtrip(display);

    while (wl_display_dispatch(display) != -1);
    return 0;
}
